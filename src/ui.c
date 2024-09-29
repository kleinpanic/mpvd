#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "dir.h"
#include "audio.h"

void init_ncurses() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
}

void end_ncurses() {
    endwin();
}

void display_songs(WINDOW *win, char **songs, int song_count, int selected) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "Select a song or directory to play/open");

    for (int i = 0; i < song_count; i++) {
        if (i == selected)
            wattron(win, A_REVERSE);
        mvwprintw(win, i + 1, 1, "%s", songs[i]);
        wattroff(win, A_REVERSE);
    }
    wrefresh(win);
}

void display_controls(WINDOW *control_win, int is_playing, const char *current_song, int volume) {
    werase(control_win);
    box(control_win, 0, 0);

    // Display current song and volume
    mvwprintw(control_win, 1, 1, "Now Playing: %s", current_song);
    mvwprintw(control_win, 2, 1, "Volume: %d%%", volume);

    // Display if song is playing or paused
    if (is_playing) {
        mvwprintw(control_win, 1, COLS - 10, "⏸ Paused");
    } else {
        mvwprintw(control_win, 1, COLS - 10, "⏵ Playing");
    }
    wrefresh(control_win);
}

void handle_input(int ch, char **songs, int *selected_song, int *song_count, WINDOW *song_win, WINDOW *control_win, int *is_playing, char *current_directory, char *current_song, int *volume) {
    switch (ch) {
        case KEY_UP:
        case 'k':
            if (*selected_song > 0) (*selected_song)--;
            break;
        case KEY_DOWN:
        case 'j':
            if (*selected_song < *song_count - 1) (*selected_song)++;
            break;
        case '\n':  // Handle directory or song selection
            if (songs[*selected_song][strlen(songs[*selected_song]) - 1] == '/') {
                char new_dir[MAX_PATH_LEN];
                snprintf(new_dir, MAX_PATH_LEN, "%s/%s", current_directory, songs[*selected_song]);
                navigate_directory(new_dir, songs, song_count);
                strcpy(current_directory, new_dir);  // Update current directory
                *selected_song = 0;  // Reset selection after entering directory
            } else {
                char song_path[MAX_PATH_LEN];
                snprintf(song_path, MAX_PATH_LEN, "%s/%s", current_directory, songs[*selected_song]);
                play_audio(song_path);
                strcpy(current_song, songs[*selected_song]);  // Update current song
                *is_playing = 1;
            }
            break;
        case ' ':  // Pause/Play toggle
            *is_playing = !(*is_playing);
            break;
        case KEY_RIGHT:  // Increase volume
            if (*volume < 100) (*volume)++;
            break;
        case KEY_LEFT:  // Decrease volume
            if (*volume > 0) (*volume)--;
            break;
    }
    display_songs(song_win, songs, *song_count, *selected_song);
    display_controls(control_win, *is_playing, current_song, *volume);
}
