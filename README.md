# CommandLineTunes
Simple audio player with a CLI.
## Features
* Quick, clean crossfade between songs to avoid those annoying clicks at the start and end of songs (in normal mode)
* Continuous playback of songs with no transition (also known as gapless playback) (in continuous mode)
* Simple, easy to use, text-only interface
* Basic volume controls and the ability to skip songs
* Plays mp3, wav and flac files
## Controls
| Key        | Action          |
|------------|-----------------|
| Tab        | Skip song       |
| Space      | Pause           |
| Equals key | Increase volume |
| Minus key  | Decrease volume |
## Arguments
| Argument                     | What it does                             |
|------------------------------|------------------------------------------|
| --continuous OR -c           | Start CLTunes in continuous mode         |
| --start-at < filename >      | Start playback at the file with that name|
| --fade-time < milliseconds > | Milliseconds of fade between songs       |
|                              | ^ Does nothing if in continuous mode. ^  |
## General information
This software is written in C++. For audio playback, it uses the Miniaudio library.
## How to install
### Linux:
Steps:
1. Download the install.sh script - `wget https://raw.githubusercontent.com/DevMa7e1/CommandLineTunes/refs/heads/main/install.sh`
2. Chmod the script - `chmod +x install.sh`
3. Run the script as root - `sudo ./install.sh`

Done! Now you can just run the `cltunes` command in a terminal and start using CommandLineTunes. To get more information on how to use CommandLineTunes, check out the **How to use** section.
### Windows:
Steps:
1. Download the install.bat script from [here](https://raw.githubusercontent.com/DevMa7e1/CommandLineTunes/refs/heads/main/install.bat) (hit ctrl+s on the page)
2. In the Windows Start Menu search bar, type cmd
3. Click on run as administrator
4. Drag the install.bat script into the cmd window
5. Hit enter
6. Restart

Done! Now you can just run the `cltunes` command in a terminal and start using CommandLineTunes. To get more information on how to use CommandLineTunes, check out the **How to use** section.
## How to use
CommandLineTunes plays every audio file in a folder in alphabetical order. If you want to make a "playlist", just copy the audio files you want in that "playlist" in an empty folder.

To start playing the audio files, just open a terminal, navigate to that directory and run the `cltunes` command.

CommandLineTunes has two modes: normal mode and continuous mode. For most songs, you're probably going to want to use the normal mode. If the songs you want to listen to transition from one another cleanly themselves, you can get a better experience with continuous mode. To use CommandLineTunes in continuous mode, just pass the -c or --continuous argument (run the command `cltunes -c`).

If you hear relatively loud "popping" sounds when one song ends and another begins in continuous mode, it most likely means that those songs are not made to be played in such a way and should be played in normal mode.
## How to build
### Linux:
Steps:
1. Install git and wget <br>
    **Debian, Ubuntu:**<br>
    `sudo apt install wget git`<br>
    **Arch Linux:**<br>
    `sudo pacman -Syu wget git`<br>
    **Fedora, RHEL, CentOS:**<br>
    `sudo dnf install wget git`<br>

2. Obtain a copy of miniaudio.h - `wget https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h`
3. Obtain a copy of the source code - `git clone https://github.com/DevMa7e1/CommandLineTunes`
4. Put the copy of miniaudio.h in the same directory as the source code - `mv miniaudio.h ./CommandLineTunes/`
5. Make sure you have installed the essential packages for building CommandLineTunes
    <br>**Debian, Ubuntu:**<br>
    `sudo apt install build-essential libc6 libc++6 libgcc-s1`<br>
    **Arch Linux:**<br>
    `sudo pacman -Syu gcc libc6 libc++6 libgcc-s1`<br>
    **Fedora, RHEL, CentOS:**<br>
    `sudo dnf install gcc libc6 libc++6 libgcc-s1`<br>
6. Go into the directory containing the source code - `cd CommandLineTunes`
7. Compile with gcc - `g++ ./miniaudio.c ./main.cpp -o cltunes -ldl -lpthread -lm`

### Windows
Steps:
1. Install Chocolatey
2. In a terminal, run `choco install mingw`
2. Download a copy of the source code from [here](https://github.com/DevMa7e1/CommandLineTunes/archive/refs/heads/main.zip)
3. Download a copy of miniaudio.h from [here](https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h) (hit ctrl+s on the page)
4. Unzip the source code
5. Move the miniaudio.h file into the directory with the source code
6. Open a terminal in that directory
7. Run the command `g++ miniaudio.c main.cpp -o cltunes.exe -static`
8. Done

### MacOS
Steps:
1. Install brew
2. In a terminal, run `brew install gcc`
3. Download a copy of the source code from [here](https://github.com/DevMa7e1/CommandLineTunes/archive/refs/heads/main.zip)
4. Download a copy of miniaudio.h from [here](https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h) (right click, Save Page As...)
5. Rename the resulting file to miniaudio.h
6. Put the copy of miniaudio.h in the same directory as the source code
8. Open a terminal and go into the directory containing the source code
8. Run the command `g++ miniaudio.c main.cpp -o cltunes -std=c++17`