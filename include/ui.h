#ifndef UI_H
#define UI_H

#include <ncurses.h>

void init_ncurses();
void end_ncurses();
void display_songs(WINDOW *win, char **songs, int song_count, int selected);
void display_controls(WINDOW *control_win, int is_playing);

// Update handle_input to accept current_directory
void handle_input(int ch, char **songs, int *selected_song, int *song_count, WINDOW *song_win, WINDOW *control_win, int *is_playing, char *current_directory);

#endif
