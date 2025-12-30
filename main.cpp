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
string sepr = "\\";
void uni_sleep(int milliseconds){
    Sleep(milliseconds);
}
#else
#include <unistd.h>
string block = "█";
string bar = "‖";
string sepr = "/";
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
const char* fade_time[1] = {"--fade-time"};
const char* preload_and_zero_transition[2] = {"--continuous", "-c"};
float fade_duration = 50;
vector<unique_ptr<ma_decoder>> decoders;
size_t decoder_i;
bool callback_signal = false;

void data_callback(ma_device *device, void* output, const void*, ma_uint32 framec){
    ma_data_source_read_pcm_frames(decoders[decoder_i].get(), output, framec, NULL);
}

static ma_data_source* next_callback_tail(ma_data_source* pDataSource){
    callback_signal = true;
    return &decoders[0];
}

int normalModePlay(set<fs::path> sorted_by_name){
    ma_result result;
    ma_sound sound;
    ma_engine engine;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        exit(result);
    }
    for (auto &entry : sorted_by_name){
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
                                exit(result);
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
                    else if(key == 'x'){
                        ma_sound_stop(&sound);
                        ma_sound_uninit(&sound);
                        ma_engine_uninit(&engine);
                        return 0;
                    }
                }
                uni_sleep(10);
            }
            ma_sound_uninit(&sound);
        }
    }
    ma_engine_uninit(&engine);
    return 0;
}

int continuousModePlay(set<fs::path> sorted_by_name){
    ma_device_config devconf = ma_device_config_init(ma_device_type_playback);
    devconf.playback.format = ma_format_f32;
    devconf.playback.channels = 2;
    devconf.sampleRate = 48000;
    devconf.dataCallback = data_callback;
    devconf.pUserData = NULL;
    ma_device device;
    if(ma_device_init(nullptr, &devconf, &device) != MA_SUCCESS){
        cout << "Fatal error in audio device init. Program cannot continue execution.";
        return 1;
    }
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, device.playback.channels, device.sampleRate);
    vector<string> names;
    decoders.reserve(sorted_by_name.size());
    names.reserve(sorted_by_name.size());
    for(auto &entry : sorted_by_name){
        auto decoder = make_unique<ma_decoder>();
        if(ma_decoder_init_file(entry.string().c_str(), &config, decoder.get()) != MA_SUCCESS){
            cout << "Fatal problem loading " << entry.string() << ". Please make sure that the file is a valid audio file and try again.\n";
            return 1;
        }
        decoders.push_back(move(decoder));
        names.push_back(entry.filename().string());
    }
    if(decoders.empty()){
        cout << "No audio files found.";
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
    while(!callback_signal){
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
            else if(key == 'x'){
                ma_device_stop(&device);
                ma_device_uninit(&device);
                for(auto &decoder : decoders){
                    ma_data_source_uninit(decoder.get());
                    ma_decoder_uninit(decoder.get());
                }
                decoders.clear();
                decoder_i = 0;
                return 0;
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
    ma_device_stop(&device);
    ma_device_uninit(&device);
    for(auto &decoder : decoders){
        ma_data_source_uninit(decoder.get());
        ma_decoder_uninit(decoder.get());
    }
    decoders.clear();
    decoder_i = 0;
    return 0;
}

set<fs::path> loadFiles(string path, int pointer = -1){
    set<fs::path> sorted_by_name;
    for (auto &entry : fs::directory_iterator(path)){
        if(entry.path().has_extension() && isOneOfTheStrings(entry.path().extension().string().c_str(), supportedExtentions, 3))
            sorted_by_name.insert(entry.path());
    }
    set<fs::path> sorted_by_name2;
    int i = 0;
    for(auto &entry : sorted_by_name){
        if(i >= pointer){
            sorted_by_name2.insert(entry);
        }
        i++;
    }
    return sorted_by_name2;
}

bool stringHasPrefix(string a, string b){
    if(a.size() < b.size()){
        return false;
    }
    else{
        bool yes = true;
        for(int i = 0; i < b.size(); i++){
            if(a[i] != b[i]){
                yes = false;
                break;
            }
        }
        return yes;
    }
}

string replaceChar(string s, char a, char b){
    for(int i = 0; i < s.size(); i++){
        if(s[i] == a) s[i] = b;
    }
    return s;
}

std::string path = std::filesystem::current_path().string();
string explorer_command = "";
int pointer = -1;
int main(int argc, char** argv){
    if(argc > 1){
        for(int i = 0; i < argc; i++){
            if(isOneOfTheStrings(argv[i], fade_time, 1)){
                fade_duration = atof(argv[i+1]);
            }
        }
    }
    displayInterface("No file", 0, 1);
    cout << "Welcome to CLTunes!\nType help for a list of commands available." << endl << endl;
    while(1){
        cout << "CLTunes@" << path << "# ";
        cin >> explorer_command;
        if(explorer_command == "ls"){
            int music_files = 0;
            for(auto &entry : fs::directory_iterator(path)){
                if(entry.is_directory()){
                    cout << entry.path().filename().string() << "/" << endl;
                }
            }
            for(auto &entry : loadFiles(path)){
                if(entry.has_extension() && isOneOfTheStrings(entry.extension().string().c_str(), supportedExtentions, 3)){
                    if(pointer != -1 && pointer == music_files)
                        cout << music_files << " - " << entry.filename().string() << " <--" << endl;
                    else
                        cout << music_files << " - " << entry.filename().string() << endl;
                    music_files++;
                }
            }
            if(music_files == 0){
                cout << "This folder contains no supported music files." << endl;
            }
        }
        else if(explorer_command == "cd"){
            string where;
            cin >> where;
            where = replaceChar(where, '/', ' ');
            string where2 = where;
            if(where.erase(0, where.size()-1) == "*"){
                where2.erase(where2.size()-1);
                for(auto &entry : fs::directory_iterator(path)){
                    if(entry.is_directory() && stringHasPrefix(entry.path().filename().string(), where2)){
                        where = entry.path().filename().string();
                        break;
                    }
                }
                if(where == "*"){
                    where = where2+"*";
                }
            }
            else{
                where = where2;
            }
            string last_path = path;
            path.append(sepr);
            path.append(where);
            if(!fs::is_directory(path)){
                path = last_path;
                where2 = where;
                if(where.erase(0, where.size()-1) == "*")
                    cout << "No directory matches the pattern " << where2 << endl;
                else
                    cout << where2 << " is not a valid directory." << endl;
            }
            if(where == ".."){
                path.erase(path.rfind(sepr[0]));
                path.erase(path.rfind(sepr[0]));
            }
            if(where == "."){
                path.erase(path.rfind(sepr[0]));
            }
        }
        else if(explorer_command == "exit"){
            exit(0);
        }
        else if(explorer_command == "clear"){
            displayInterface("No file", 0, 1);
        }
        else if(explorer_command == "play"){
            set<fs::path> sorted_by_name = loadFiles(path, pointer);
            cin >> explorer_command;
            if(explorer_command == "c"){
                continuousModePlay(sorted_by_name);
                displayInterface("No file", 0, 1);
            }
            else if(explorer_command == "n"){
                normalModePlay(sorted_by_name);
                displayInterface("No file", 0, 1);
            }
            else
                cout << "Invalid option." << endl;
        }
        else if(explorer_command == "point"){
            set<fs::path> sorted_by_name = loadFiles(path);
            cin >> pointer;
            if(pointer < 0 || pointer >= sorted_by_name.size()){
                pointer = -1;
            }
        }
        else if(explorer_command == "pointer"){
            if(pointer != -1){
                set<fs::path> sorted_by_name = loadFiles(path);
                std::set<fs::path>::iterator it = sorted_by_name.begin();
                std::advance(it, pointer);
                cout << (*it).filename().string() << endl;
            }
            else{
                cout << "There is no file being pointed to." << endl;
            }
        }
        else if(explorer_command == "help"){
            cout << "CLTunes CLI commands:" << endl;
            cout << "ls - list directories and music files" << endl;
            cout << "cd - change directory" << endl;
            cout << "point # - change the pointer's number - replace the # with a number" << endl;
            cout << "pointer - print out the name of the file being pointed to" << endl;
            cout << "play n - play the music files in the directory in normal mode" << endl;
            cout << "play c - play the music files in the directory in continuous mode" << endl;
            cout << "clear - clear screen" << endl;
            cout << "exit - exit CLTunes" << endl;
        }
        else{
            cout << "Invalid command." << endl;
        }
    }
}