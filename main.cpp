#include <iostream>
#include "miniaudio.h"
#include <cmath>
#include <string>
#include <filesystem>
#include <set>
#include "keypress.hpp"
namespace fs = std::filesystem;

using namespace std;

bool isOneOfTheStrings(const char* a, const char** b, int nob){
    bool yes = false;
    for(int j = 0; j < nob; j++){
        int i = 0;
        bool yes2 = true;
        while(a[i] != 0x00 && b[j][i] != 0x00){
            if(a[i] != b[j][i]){
                yes2 = false;
            }
            i++;
        }
        if(yes2){
            yes = true;
        }
    }
    return yes;
}

string cutName(const char* name, int index){
    string a;
    for(int i = 0; i < 22; i++){
        a += name[i+index];
    }
    return a;
}

void uni_clear(){
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

int nsindx = 0;
void displayInterface(const char* name, float cursor, float lenght){
    uni_clear();
    cout << "\n-- CLTunes --\n";
    int namelen = 0;
    while(name[namelen] != 0x00) namelen++;
    for(int i = 0; i < 24; i++){
        cout << "=";
    }
    if(namelen > 22)
        cout << endl << "‖" << cutName(name, nsindx) << "‖" << endl;
    else{
        cout << endl << "‖" << name;
        for(int i = 0; i < 22-namelen; i++){
            cout << " ";
        }
        cout << "‖" << endl;
    }
    if(namelen-22 > 0)
        nsindx = (nsindx+1)%(namelen-21);
    else
        nsindx = 0;
    cout << "‖>";
    for(int i = 0; i < 20; i++){
        if(cursor/lenght > 0.05*(i+1)){
            cout << "█";
        }
        else{
            cout << " ";
        }
    }
    cout << "<‖" << endl;
    for(int i = 0; i < 24; i++){
        cout << "=";
    }
    cout << endl;
}

const char* supportedExtentions[3] = {".wav", ".flac", ".mp3"};
const char* start_at[1] = {"--start-at"};
const char* fade_time[1] = {"--fade-time"};
bool startAtDone = false;
float fade_duration = 50;
string specified = "";

int main(int argc, char** argv){
    ma_result result;
    ma_sound sound;
    ma_engine engine;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return result;
    }
    std::string path = std::filesystem::current_path();
    set<fs::path> sorted_by_name;

    for (auto &entry : fs::directory_iterator(path))
        sorted_by_name.insert(entry.path());
    if(argc > 1){
        for(int i = 0; i < argc; i++){
            if(isOneOfTheStrings(argv[i], start_at, 1) && !startAtDone){
                specified = argv[i+1];
            }
            else if(isOneOfTheStrings(argv[i], fade_time, 1)){
                fade_duration = atof(argv[i+1]);
            }
        }
    }
    for (auto &entry : sorted_by_name){
        if(specified != "" && specified != entry.filename().string() && !startAtDone)
            continue;
        else                        
            startAtDone = true;
        if(entry.has_extension() && isOneOfTheStrings(entry.extension().string().c_str(), supportedExtentions, 3)){
            result = ma_sound_init_from_file(&engine, entry.filename().c_str(), MA_SOUND_FLAG_NO_PITCH+MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &sound);
            if (result != MA_SUCCESS) {
                return result;
            }
            float length;
            ma_sound_set_start_time_in_milliseconds(&sound, 0);
            ma_sound_get_length_in_seconds(&sound, &length);
            ma_sound_set_fade_in_milliseconds(&sound, 0, 1, fade_duration);
            ma_sound_start(&sound);
            bool setFadeOut = false;
            float lcursor = 0.0;
            bool paused = false;
            float cursor_paused = 0.0;
            bool waitASecondPlease = false;
            while(ma_sound_is_playing(&sound) || paused || waitASecondPlease){
                if(!paused){
                    float cursor;
                    ma_sound_get_cursor_in_seconds(&sound, &cursor);
                    if(lcursor+1 < cursor){
                        if(waitASecondPlease){
                            waitASecondPlease = false;
                        }
                        displayInterface(entry.filename().c_str(), cursor, length);
                        lcursor = cursor;
                    }
                    if (cursor + fade_duration/1000 + 0.02 > length && !setFadeOut){
                        ma_sound_set_fade_in_milliseconds(&sound, -1, 0, fade_duration);
                        setFadeOut = true;
                    }
                }
                if (keyPressed()){
                    char key = getKey();
                    if(key == ' ' && !waitASecondPlease){
                        if(!paused){
                            paused = true;
                            ma_sound_stop_with_fade_in_milliseconds(&sound, 250);
                            ma_sound_get_cursor_in_seconds(&sound, &cursor_paused);
                            cout << "*Paused*\n";
                        }
                        else{
                            ma_sound_uninit(&sound);
                            paused = false;
                            result = ma_sound_init_from_file(&engine, entry.filename().c_str(), 0, NULL, NULL, &sound);
                            if (result != MA_SUCCESS) {
                                return result;
                            }
                            ma_sound_set_fade_in_milliseconds(&sound, 0, 1, 250);
                            ma_sound_seek_to_second(&sound, cursor_paused);
                            ma_sound_start(&sound);
                            waitASecondPlease = true;
                        }
                    }
                    else if(key == 0x09)
                    {
                        ma_sound_stop_with_fade_in_milliseconds(&sound, 250);
                    }
                    else if(key == '-'){
                        if(ma_sound_get_volume(&sound) > 0)
                            ma_sound_set_volume(&sound, ma_sound_get_volume(&sound)-0.05);
                        cout << "Volume: " << round(ma_sound_get_volume(&sound)*100) << "%" << endl;
                    }
                    else if(key == '='){
                        if(ma_sound_get_volume(&sound) < 1)
                            ma_sound_set_volume(&sound, ma_sound_get_volume(&sound)+0.05);
                        cout << "Volume: " << round(ma_sound_get_volume(&sound)*100) << "%" << endl;
                    }
                }
            }
            ma_sound_uninit(&sound);
            uni_clear();
            cout << entry.filename().c_str() << " finished playing.";
        }
    }
}