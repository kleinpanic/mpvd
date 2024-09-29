#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include "dir.h"

char* get_music_directory() {
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    char *music_dir = malloc(strlen(homedir) + strlen("/Music/mpvd") + 1);
    strcpy(music_dir, homedir);
    strcat(music_dir, "/Music/mpvd");
    return music_dir;
}

char** load_songs(int *count, const char *music_dir) {
    DIR *d;
    struct dirent *dir;
    d = opendir(music_dir);
    if (!d) {
        perror("Failed to open music directory");
        exit(1);
    }

    char **songs = malloc(sizeof(char*) * 100);
    int index = 0;

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
            songs[index] = strdup(dir->d_name);
            index++;
        } else if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            char dir_name[512];
            snprintf(dir_name, sizeof(dir_name), "%s/", dir->d_name);
            songs[index] = strdup(dir_name);  // Append '/' to indicate a directory
            index++;
        }
    }
    closedir(d);
    *count = index;
    return songs;
}

void navigate_directory(const char *dir, char **songs, int *song_count) {
    free(songs);  // Free old song list
    songs = load_songs(song_count, dir);  // Load new song list
}
