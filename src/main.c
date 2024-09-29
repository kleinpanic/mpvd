// src/main.c

#include <ncurses.h>
#include "audio.h"
#include "ui.h"
#include "dir.h"

int main() {
    // Initialize ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    // Initialize UI and audio
    init_ui();
    init_audio();

    // Load the directory contents
    load_directory("/Music/mpvd");

    int ch;
    while ((ch = getch()) != 'q') {
        // Handle navigation and controls
        switch (ch) {
            case KEY_UP:
                navigate_up();
                break;
            case KEY_DOWN:
                navigate_down();
                break;
            case '\n':  // Enter key
                select_item();
                break;
            case ' ':  // Spacebar
                toggle_playback();
                break;
            case KEY_LEFT:
                decrease_volume();
                break;
            case KEY_RIGHT:
                increase_volume();
                break;
        }
        update_ui();
    }

    // Cleanup
    cleanup_ui();
    cleanup_audio();
    endwin();

    return 0;
}
