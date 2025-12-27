#include <iostream>
#include "miniaudio.h"
#include <cmath>
#include <string>
#include <filesystem>
#include <set>
#include <vector>
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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
string block = "]";
string bar = "|";
void sleep(float seconds){
    Sleep(seconds*1000);
}
#else
#include <unistd.h>
string block = "█";
string bar = "‖";
#endif

int nsindx = 0;
void displayInterface(const char* name, float cursor, float lenght, bool notr = false){
    uni_clear();
    cout << "\n-- CLTunes --\n";
    int namelen = 0;
    while(name[namelen] != 0x00) namelen++;
    for(int i = 0; i < 24; i++){
        cout << "=";
    }
    if(namelen-22 > 0)
        nsindx = (nsindx+1)%(namelen-21);
    else
        nsindx = 0;
    if(namelen > 22)
        cout << endl << bar << cutName(name, nsindx) << bar << endl;
    else{
        cout << endl << bar << name;
        for(int i = 0; i < 22-namelen; i++){
            cout << " ";
        }
        cout << bar << endl;
    }
    cout << bar << ">";
    if(!notr)
    for(int i = 0; i < 20; i++){
        if(cursor/lenght > 0.05*(i+1)){
            cout << block;
        }
        else{
            cout << " ";
        }
    }
    else{
        cout << "--CONTINUOUS PLAY!--";
    }
    cout << "<" << bar << endl;
    for(int i = 0; i < 24; i++){
        cout << "=";
    }
    cout << endl;
}

const char* supportedExtentions[3] = {".wav", ".flac", ".mp3"};
const char* start_at[1] = {"--start-at"};
const char* fade_time[1] = {"--fade-time"};
const char* preload_and_zero_transition[1] = {"--no-transition"};
bool startAtDone = false;
float fade_duration = 50;
string specified = "";
bool preload = false;
vector<ma_decoder> decoders;
size_t decoder_i;
bool basicallyDone = false;

void data_callback(ma_device* device, void* output, const void*, ma_uint32 framec){
    float *out = static_cast<float*>(output);
    ma_uint32 frames_r = framec;
    while(frames_r > 0 && decoder_i < decoders.size()){
        ma_uint64 frames_read = 0;
        ma_decoder_read_pcm_frames(&decoders[decoder_i], out, frames_r, &frames_read);
        if(frames_read == 0){
            decoder_i++;
            continue;
        }
        out += frames_read * device->playback.channels;
        frames_r -= frames_read;
    }
    if(frames_r > 0){
        fill(out, out + frames_r * device->playback.channels, 0.0f);
        basicallyDone = true;
    }
}

int main(int argc, char** argv){
    ma_result result;
    ma_sound sound;
    ma_engine engine;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return result;
    }
    std::string path = std::filesystem::current_path().string();
    set<fs::path> sorted_by_name;

    for (auto &entry : fs::directory_iterator(path)){
        if(entry.path().has_extension() && isOneOfTheStrings(entry.path().extension().string().c_str(), supportedExtentions, 3))
            sorted_by_name.insert(entry.path());
    }
    if(argc > 1){
        for(int i = 0; i < argc; i++){
            if(isOneOfTheStrings(argv[i], start_at, 1) && !startAtDone){
                specified = argv[i+1];
            }
            else if(isOneOfTheStrings(argv[i], fade_time, 1)){
                fade_duration = atof(argv[i+1]);
            }
            else if(isOneOfTheStrings(argv[i], preload_and_zero_transition, 1)){
                preload = true;
            }
        }
    }
    if(!preload){
    for (auto &entry : sorted_by_name){
        if(specified != "" && specified != entry.filename().string() && !startAtDone)
            continue;
        else                        
            startAtDone = true;
        if(true){
            result = ma_sound_init_from_file(&engine, entry.filename().string().c_str(), MA_SOUND_FLAG_NO_PITCH+MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &sound);
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
                        displayInterface(entry.filename().string().c_str(), cursor, length);
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
                            result = ma_sound_init_from_file(&engine, entry.filename().string().c_str(), 0, NULL, NULL, &sound);
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
                sleep(0.01);
            }
            ma_sound_uninit(&sound);
            uni_clear();
            cout << entry.filename().string().c_str() << " finished playing.";
        }
    }}
    else{
        ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 2, 0);
        vector<string> names;
        for(auto &entry : sorted_by_name){
            ma_decoder decoder;
            if(ma_decoder_init_file(entry.string().c_str(), &config, &decoder) != MA_SUCCESS){
                cout << "Fatal problem loading " << entry.string() << ". Please make sure that the file is a valid audio file and try again.\n";
                return 1;
            }
            decoders.push_back(decoder);
            names.push_back(entry.filename().string());
        }
        if(decoders.empty()){
            cout << "No audio files found. Program cannot continue execution.";
            return 0;
        }
        ma_device_config devconf = ma_device_config_init(ma_device_type_playback);
        devconf.playback.format = ma_format_f32;
        devconf.playback.channels = 2;
        devconf.sampleRate = decoders[0].outputSampleRate;
        devconf.dataCallback = data_callback;
        devconf.pUserData = NULL;

        ma_device device;
        if(ma_device_init(nullptr, &devconf, &device) != MA_SUCCESS){
            cout << "Fatal error in audio device init. Program cannot continue execution.";
        }
        ma_device_start(&device);
        while(!basicallyDone){
            displayInterface(names[decoder_i].c_str(), 0, 0, true);
            sleep(1);
        }
        ma_device_uninit(&device);
        for(auto &decoder : decoders){
            ma_decoder_uninit(&decoder);
        }
        return 0;
    }
}