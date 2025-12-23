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
        nsindx = (nsindx+1)%(namelen-22);
    else
        nsindx = 0;
    cout << "‖|";
    for(int i = 0; i < 20; i++){
        if(cursor/lenght > 0.05*(i+1)){
            cout << "█";
        }
        else{
            cout << " ";
        }
    }
    cout << "|‖" << endl;
    for(int i = 0; i < 24; i++){
        cout << "=";
    }
    cout << endl;
}

const char* supportedExtentions[3] = {".wav", ".flac", ".mp3"};

int main(){
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
    
    for (auto &entry : sorted_by_name){
        if(entry.has_extension() && isOneOfTheStrings(entry.extension().string().c_str(), supportedExtentions, 3)){
            result = ma_sound_init_from_file(&engine, entry.filename().c_str(), 0, NULL, NULL, &sound);
            if (result != MA_SUCCESS) {
                return result;
            }
            float length;
            ma_sound_set_start_time_in_milliseconds(&sound, 0);
            ma_sound_get_length_in_seconds(&sound, &length);
            ma_sound_set_fade_in_milliseconds(&sound, 0, 1, 50);
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
                    if (cursor + 0.07 > length && !setFadeOut){
                        ma_sound_set_fade_in_milliseconds(&sound, -1, 0, 50);
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
                    else if (key == '=')
                    {
                        ma_sound_stop_with_fade_in_milliseconds(&sound, 250);
                    }
                    
                }
            }
            ma_sound_uninit(&sound);
            uni_clear();
            cout << entry.filename().c_str() << " finished playing.";
        }
    }
}