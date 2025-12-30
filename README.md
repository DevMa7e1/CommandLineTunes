# CommandLineTunes
Simple audio player with a CLI.
## Features
* Quick, clean crossfade between songs to avoid those annoying clicks at the start and end of songs (in normal mode)
* Continuous playback of songs with no transition (also known as gapless playback) (in continuous mode)
* Simple, easy to use, text-only CLI
* Basic volume controls and the ability to skip songs (in normal mode)
* Plays mp3, wav and flac files
## General information
This software is written in C++. For audio playback, it uses the Miniaudio library.
## How to install
### Linux:
Steps:
1. Download the install.sh script - `wget https://raw.githubusercontent.com/DevMa7e1/CommandLineTunes/refs/heads/main/install.sh`
2. Chmod the script - `chmod +x install.sh`
    * If you are using a distro that does not have the `apt` command, please edit line 6 in install.sh and replace `apt` with your distro's package manager.
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
When you run the `cltunes` command, you're met with the CLTunes CLI. From here, you can navigate to folders and start playback.

### In the CLTunes CLI:
#### `help` command:
To see a list of commands and what they do, just run the command `help`.
#### `ls` command:
To see if the directory you're currently in has any audio files and what other directories it contains, just run the command `ls`. The `ls` command also showes the ID of each music file.

If you set CLTunes to point to a file, the `ls` command will put a little arrow (<--) in front of the file being pointed to.
#### `cd` command:
The `cd` command changes the directory being interacted with in the CLI. You can only "`cd`" into directories available in the currently being interacted with directory.

If the name of a directory you wanna andvance into contains spaces, replace the spaces with a / (e.g., to go into "folder name with spaces", you'd run the command `cd folder/name/with/spaces`). Otherwise, the CLTunes CLI command interpreter would just interpret the other segments of the name as seperate commands (e.g. `cd folder name with spaces` gets interpreted as `cd folder` then `name` then `with` then `spaces`).

If you know the start of the name of a folder but not all of it, you can just type what you know and put a * at the end. Doing this will make the CLTunes CLI command interpreter pick the first folder it comes across starting with the text before the *.
#### `point` and `pointer` command:
To start playback at a specific file, you have to make CLTunes "point" at it.

To get the file at which CLTunes is pointing, run the `pointer` command.

To point to a file, just get the audio file's ID from the `ls` command and pass it to the `point` command.
Example:
```
CLTunes@/home/user/Music# ls
0 - play.wav
1 - stay.wav
2 - wait.wav
3 - love.wav
CLTunes@/home/dev/CommandLineTunes# point 2
CLTunes@/home/dev/CommandLineTunes# pointer
wait.wav
```
#### `play` command:
To start playback of the audio files in the currently being interacted with directory, just run one of the play commands: `play n` or `play c`

`play n` starts playback in the normal mode. Most songs should be played with the normal mode. Though, if you suspect that the songs you're listening to transition from one another themselves, you should try playing them with `play c`.

`play c` starts playback in the continuous mode. If when listening to songs in this mode you hear relatively loud "popping sounds", it most likely means that the songs you're listening to aren't made to be played in such a mode or that they aren't being played in the correct order. The way continuous mode works might better be known under the name of "gapless playback".
#### `clear` command:
Clears the terminal screen.

Have too much text on the screen and want to clear it up? Run the `clear` command.
#### `exit` command:
Exits the CLTunes CLI.

### After running one of the `play` commands:
You can control the playback by pressing singular keys.

| Key        | Action                           |
|------------|----------------------------------|
| Tab        | Skip song (in normal mode)       |
| Space      | Pause                            |
| Equals key | Increase volume                  |
| Minus key  | Decrease volume                  |
| X key      | Stop playback, return to the CLI |

### Extra
If you want to change the time it takes to fade in and out between songs in normal mode, just pass the --fade-time argument and the number of milliseconds of fading when running CLTunes (e.g. `cltunes --fade-time 250`).

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