// For full transparency
// The code contained within this file has been generated with ChatGPT

#ifndef KEYPRESS_HPP
#define KEYPRESS_HPP

#include <iostream>

#ifdef _WIN32
    #include <conio.h>
    // Checks if a key has been pressed (non-blocking) on Windows
    inline bool keyPressed() {
        return _kbhit();
    }

    // Reads the pressed key (Windows)
    inline char getKey() {
        return _getch();
    }

#else // POSIX (Linux, macOS)
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>

    inline bool keyPressed() {
        termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if (ch != EOF) {
            ungetc(ch, stdin);
            return true;
        }
        return false;
    }

    inline char getKey() {
        return getchar();
    }

#endif // _WIN32

#endif // KEYPRESS_HPP
