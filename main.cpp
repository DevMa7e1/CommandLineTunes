#include <iostream>
#include "miniaudio.h"
#include <cmath>
#include <string>
#include <filesystem>
#include <set>
#include <vector>
#include "keypress.hpp"
#include <cstring>
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
        if(a[i] != 0x00 || b[j][i] != 0x00) yes2 = false;
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
void uni_sleep(int milliseconds){
    Sleep(milliseconds);
}
#else
#include <unistd.h>
string block = "█";
string bar = "‖";
void uni_sleep(int milliseconds){
    usleep(milliseconds*1000);
}
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
        for(int i = 0; i < 20; i++){
            if(cursor/lenght > 0.05*(i+1)){
                cout << block;
            }
            else{
                cout << "--CONTINUOUS MODE!--"[i];
            }
        }
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
const char* preload_and_zero_transition[2] = {"--continuous", "-c"};
bool startAtDone = false;
float fade_duration = 50;
string specified = "";
bool preload = false;
vector<unique_ptr<ma_decoder>> decoders;
size_t decoder_i;

void data_callback(ma_device *device, void* output, const void*, ma_uint32 framec){
    ma_data_source_read_pcm_frames(decoders[decoder_i].get(), output, framec, NULL);
}

static ma_data_source* next_callback_tail(ma_data_source* pDataSource){
    exit(0);
    return &decoders[0];
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
            else if(isOneOfTheStrings(argv[i], preload_and_zero_transition, 2)){
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
                uni_sleep(10);
            }
            ma_sound_uninit(&sound);
            uni_clear();
            cout << entry.filename().string().c_str() << " finished playing.";
        }
    }}
    else{
        set<fs::path> audio_files;
        bool found = false;
        for(auto &entry : sorted_by_name){
            if(specified == ""){
                audio_files.insert(entry);
            }
            else if(specified == entry.filename().string()){
                found = true;
                audio_files.insert(entry);
            }
            else if(found){
                audio_files.insert(entry);
            }
        }
        ma_device_config devconf = ma_device_config_init(ma_device_type_playback);
        devconf.playback.format = ma_format_f32;
        devconf.playback.channels = 2;
        devconf.sampleRate = 48000;
        devconf.dataCallback = data_callback;
        devconf.pUserData = NULL;

        ma_device device;
        if(ma_device_init(nullptr, &devconf, &device) != MA_SUCCESS){
            cout << "Fatal error in audio device init. Program cannot continue execution.";
        }
        ma_decoder_config config = ma_decoder_config_init(ma_format_f32, device.playback.channels, device.sampleRate);
        vector<string> names;
        decoders.reserve(sorted_by_name.size());
        names.reserve(sorted_by_name.size());
        for(auto &entry : audio_files){
            auto decoder = make_unique<ma_decoder>();
            if(ma_decoder_init_file(entry.string().c_str(), &config, decoder.get()) != MA_SUCCESS){
                cout << "Fatal problem loading " << entry.string() << ". Please make sure that the file is a valid audio file and try again.\n";
                return 1;
            }
            decoders.push_back(move(decoder));
            names.push_back(entry.filename().string());
        }
        if(decoders.empty()){
            cout << "No audio files found. Program cannot continue execution.";
            return 0;
        }
        for(int i = 0; i + 1 < decoders.size(); i++){
            ma_data_source_set_next(decoders[i].get(), decoders[i+1].get());
        }
        ma_data_source_set_next_callback(decoders[decoders.size()-1].get(), next_callback_tail);
        ma_device_start(&device);
        int cycle = 0;
        bool fading_out = false;
        bool fading_in = false;
        float fade_cycle = 0;
        float volume = 1.0f;
        while(1){
            float length;
            float cursor;
            float dif = 0;
            ma_data_source_get_length_in_seconds(decoders[decoder_i].get(), &length);
            ma_data_source_get_cursor_in_seconds(decoders[decoder_i].get(), &cursor);
            if(cycle == 0){
                displayInterface(names[decoder_i].c_str(), cursor-dif, length, true);
                if(!ma_device_is_started(&device)) cout << "*Paused*";
            }
            if(keyPressed()){
                char key = getKey();
                if(key == ' '){
                    if(ma_device_is_started(&device))
                        fading_out = true;
                    else{
                        ma_device_start(&device);
                        ma_device_set_master_volume(&device, 0);
                        fading_in = true;
                    }
                }
                else if(key == '-'){
                    if(volume > 0)
                        volume -= 0.05;
                    if(volume < 0.05)
                        volume = 0;
                    cout << "Volume: " << round(volume*100) << "%" << endl;
                    ma_device_set_master_volume(&device, volume);
                }
                else if(key == '='){
                    if(volume < 1)
                        volume += 0.05;
                    if(volume > 0.95)
                        volume = 1;
                    cout << "Volume: " << round(volume*100) << "%" << endl;
                    ma_device_set_master_volume(&device, volume);
                }
            }
            if(fading_in){
                fade_cycle += 1;
                if(fade_cycle >= 100){ ma_device_set_master_volume(&device, volume); fading_in = false; fade_cycle = 0;}
                else ma_device_set_master_volume(&device, fade_cycle/100.0f * volume);
            }
            if(fading_out){
                fade_cycle += 1;
                if(fade_cycle >= 100){ ma_device_set_master_volume(&device, 0); fading_out = false; fade_cycle = 0; ma_device_stop(&device);}
                else ma_device_set_master_volume(&device, (1.0f - fade_cycle/100.0f) * volume);
            }
            cycle = (cycle+1) % 100;
            if(cursor >= length){
                decoder_i++;
                dif = length;
            }
            uni_sleep(10);
        }
        ma_device_uninit(&device);
        for(auto &decoder : decoders){
            ma_decoder_uninit(decoder.get());
        }
        return 0;
    }
}