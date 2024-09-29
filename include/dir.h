#ifndef DIR_H
#define DIR_H

#define MAX_PATH_LEN 512  // Declare MAX_PATH_LEN for use across files

char* get_music_directory();
char** load_songs(int *count, const char *music_dir);
void navigate_directory(const char *dir, char **songs, int *song_count);  // Declare navigate_directory

#endif
