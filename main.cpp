#include <iostream>
#include "miniaudio.h"
#include <cmath>

using namespace std;

int main(){
    ma_result result;
    ma_sound sound;
    ma_engine engine;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return result;
    }
    result = ma_sound_init_from_file(&engine, "01.wav", 0, NULL, NULL, &sound);
    if (result != MA_SUCCESS) {
        return result;
    }
    float length;
    ma_sound_get_length_in_seconds(&sound, &length);
    ma_sound_set_fade_in_milliseconds(&sound, 0, 1, 50);
    ma_sound_start(&sound);
    bool setFadeOut = false;
    float lcursor = 0.0;
    while(ma_sound_is_playing(&sound)){
        float cursor;
        ma_sound_get_cursor_in_seconds(&sound, &cursor);
        if(lcursor+10 < cursor){
            cout << "\n" << round(cursor) << " / " << round(length);
            lcursor = cursor;
        }
        if (cursor + 0.07 > length && !setFadeOut){
            ma_sound_set_fade_in_milliseconds(&sound, -1, 0, 50);
            setFadeOut = true;
        }
    }
    ma_sound_uninit(&sound);

    result = ma_sound_init_from_file(&engine, "02.wav", 0, NULL, NULL, &sound);
    if (result != MA_SUCCESS) {
        return result;
    }
    length = 0.0;
    lcursor = 0.0;
    ma_sound_get_length_in_seconds(&sound, &length);
    ma_sound_set_fade_in_milliseconds(&sound, 0, 1, 50);
    ma_sound_start(&sound);
    setFadeOut = false;
    while(ma_sound_is_playing(&sound)){
        float cursor;
        ma_sound_get_cursor_in_seconds(&sound, &cursor);
        if(lcursor+10 < cursor){
            cout << "\n" << round(cursor) << " / " << round(length);
            lcursor = cursor;
        }
        if (cursor + 0.07 > length && !setFadeOut){
            ma_sound_set_fade_in_milliseconds(&sound, -1, 0, 50);
            setFadeOut = true;
        }
    }
    ma_sound_uninit(&sound); 
    
}