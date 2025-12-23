#include <iostream>
#include "miniaudio.h"
#include <cmath>
#include <string>
#include <filesystem>
#include <set>
namespace fs = std::filesystem;

using namespace std;

bool sameString(const char* a, const char* b){
    int i = 0;
    bool yes = true;
    while(a[i] != 0x00 && b[i] != 0x00){
        if(a[i] != b[i]){
            yes = false;
        }
        i++;
    }
    return yes;
}

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
        if(entry.has_extension() && sameString(entry.extension().string().c_str(), ".wav")){
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
            while(ma_sound_is_playing(&sound)){
                float cursor;
                ma_sound_get_cursor_in_seconds(&sound, &cursor);
                if(lcursor+0.01 < cursor){ //Very stupid but for some reason if I wait more it just doesn't work and I honestly don't know why yet
                    cout << "Currently playing " << entry.filename().c_str() << ": " << roundf(cursor) << "s out of " << round(length) << "s.\r";
                    lcursor = cursor;
                }
                if (cursor + 0.07 > length && !setFadeOut){
                    ma_sound_set_fade_in_milliseconds(&sound, -1, 0, 50);
                    setFadeOut = true;
                }
            }
            ma_sound_uninit(&sound);
        }
    }
}