# CommandLineTunes
Simple audio player with a CLI.
## Features
* Quick, clean crossfade between songs to avoid those annoying clicks at the start and end of songs
* Simple, easy to use, text-only interface
* Basic volume controls and the ability to skip songs
* Plays mp3, wav and flac files
## Controls
| Key | Action |
|-----|--------|
| Tab | Skip song |
| Space | Pause |
| Equals key | Increase volume |
| Minus key | Decrease volume |
## Arguments
| Argument                     | What it does                             |
|------------------------------|------------------------------------------|
| --start-at < filename >      | Start playback at the file with that name|
| --fade-time < milliseconds > | Milliseconds of fade between songs       |
## General information
This software is written in C++. For audio playback, it uses the Miniaudio library.
## How to build
### Linux:
Steps:
1. Obtain a copy of miniaudio.h - `wget https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h`
2. Obtain a copy of the source code - `git clone https://github.com/DevMa7e1/CommandLineTunes`
3. Put the copy of miniaudio.h in the same directory as the source code - `mv miniaudio.h ./CommandLineTunes/`
4. Go into the directory containing the source code - `cd CommandLineTunes`
5. Compile with g++ - `g++ ./miniaudio.c ./main.cpp -o main.out -ldl -lpthread -lm`

### To continue ...