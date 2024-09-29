#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <alsa/asoundlib.h>

#define MAX_SONGS 100
#define MAX_PATH_LEN 512

// Function declarations
void init_ncurses();
void end_ncurses();
void display_songs(WINDOW *win, char **songs, int song_count, int selected);
char** load_songs(int *count, const char *music_dir);
char* get_music_directory();
void display_controls(WINDOW *control_win, int is_playing);
void play_audio(const char *file_path);
void stop_audio();
void add_to_queue(const char *song);
void play_next_in_queue();
void navigate_directory(const char *dir, char **songs, int *song_count);

// Global variables
char *queue[MAX_SONGS];
int queue_size = 0;
int queue_position = 0;
int is_playing = 0;
char current_directory[MAX_PATH_LEN];

int main() {
    int song_count;
    char **songs;
    int selected_song = 0;
    int ch;

    // Initialize music directory
    strcpy(current_directory, get_music_directory());
    songs = load_songs(&song_count, current_directory);

    // Initialize ncurses
    init_ncurses();

    // Set up windows
    int half_height = LINES / 2;
    int width = COLS;
    
    WINDOW *song_win = newwin(half_height, width, 0, 0);
    WINDOW *control_win = newwin(half_height, width, half_height, 0);

    // Display initial songs and controls
    display_songs(song_win, songs, song_count, selected_song);
    display_controls(control_win, is_playing);

    // Main loop to handle input
    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
            case 'k':
                if (selected_song > 0) selected_song--;
                break;
            case KEY_DOWN:
            case 'j':
                if (selected_song < song_count - 1) selected_song++;
                break;
            case '\n':
                if (songs[selected_song][strlen(songs[selected_song]) - 1] == '/') {
                    // Enter directory
                    char new_dir[MAX_PATH_LEN];
                    snprintf(new_dir, MAX_PATH_LEN, "%s/%s", current_directory, songs[selected_song]);
                    navigate_directory(new_dir, songs, &song_count);
                    selected_song = 0;  // Reset selection
                } else {
                    if (is_playing) {
                        // Add to queue if something is playing
                        add_to_queue(songs[selected_song]);
                    } else {
                        // Play selected song immediately
                        char song_path[MAX_PATH_LEN];
                        snprintf(song_path, MAX_PATH_LEN, "%s/%s", current_directory, songs[selected_song]);
                        play_audio(song_path);
                        is_playing = 1;
                    }
                }
                break;
            case ' ': // Pause/play functionality
                if (is_playing) {
                    stop_audio();
                    is_playing = 0;
                } else {
                    play_next_in_queue();
                    is_playing = 1;
                }
                break;
            case KEY_RIGHT:
            case 'l': // Fast-forward (skipping in the queue)
                play_next_in_queue();
                break;
            case KEY_LEFT:
            case 'h': // Go back in the queue (not implemented yet)
                break;
            case KEY_NPAGE:
            case 'L': // Skip track
                play_next_in_queue();
                break;
            case KEY_PPAGE:
            case 'H': // Go back track (not implemented yet)
                break;
        }
        display_songs(song_win, songs, song_count, selected_song);  // Update song selection
        display_controls(control_win, is_playing);  // Update controls display
    }

    // Clean up ncurses
    end_ncurses();
    return 0;
}

void init_ncurses() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
}

void end_ncurses() {
    endwin();
}

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
    
    char **songs = malloc(sizeof(char*) * MAX_SONGS);
    int index = 0;

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
            songs[index] = strdup(dir->d_name);
            index++;
        } else if (dir->d_type == DT_DIR) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                char dir_name[MAX_PATH_LEN];
                snprintf(dir_name, MAX_PATH_LEN, "%s/", dir->d_name);
                songs[index] = strdup(dir_name);
                index++;
            }
        }
    }
    closedir(d);
    *count = index;
    return songs;
}

void display_songs(WINDOW *win, char **songs, int song_count, int selected) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "Select a song or directory to play/open (Up/Down to navigate, Enter to select)");
    
    for (int i = 0; i < song_count; i++) {
        if (i == selected)
            wattron(win, A_REVERSE);
        mvwprintw(win, i + 1, 1, "%s", songs[i]);
        wattroff(win, A_REVERSE);
    }
    wrefresh(win);
}

void display_controls(WINDOW *control_win, int is_playing) {
    werase(control_win);
    box(control_win, 0, 0);
    
    // Display Unicode controls for play/pause, forward, and back
    if (is_playing) {
        mvwprintw(control_win, 1, 1, "⏸ Pausing");
    } else {
        mvwprintw(control_win, 1, 1, "⏵ Playing");
    }
    mvwprintw(control_win, 1, 20, "⏪ Previous");
    mvwprintw(control_win, 1, 40, "⏩ Next");
    wrefresh(control_win);
}

void navigate_directory(const char *dir, char **songs, int *song_count) {
    strcpy(current_directory, dir);
    songs = load_songs(song_count, current_directory);
}

void add_to_queue(const char *song) {
    if (queue_size < MAX_SONGS) {
        queue[queue_size] = strdup(song);
        queue_size++;
    }
}

void play_next_in_queue() {
    if (queue_position < queue_size) {
        char song_path[MAX_PATH_LEN];
        snprintf(song_path, MAX_PATH_LEN, "%s/%s", current_directory, queue[queue_position]);
        play_audio(song_path);
        queue_position++;
    } else {
        // Queue is empty
        stop_audio();
    }
}

void play_audio(const char *file_path) {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = 44100; // Default sample rate
    int dir;
    snd_pcm_uframes_t frames;
    int rc;

    // Open PCM device for playback
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open PCM device: %s\n", snd_strerror(rc));
        return;
    }

    // Allocate a hardware parameters object
    snd_pcm_hw_params_alloca(&params);

    // Fill it in with default values
    snd_pcm_hw_params_any(handle, params);

    // Set the desired hardware parameters
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);

    // Write the parameters to the driver
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "Unable to set HW parameters: %s\n", snd_strerror(rc));
        return;
    }

    // Playback the audio (This is a simplified example, you would actually need to read the file)
    unsigned char *buffer = malloc(128 * 4);
    memset(buffer, 0, 128 * 4);

    for (int i = 0; i < 100; ++i) {
        rc = snd_pcm_writei(handle, buffer, 128);
        if (rc == -EPIPE) {
            // EPIPE means underrun
            fprintf(stderr, "Underrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "Error writing to PCM device: %s\n", snd_strerror(rc));
        }
    }

    // Cleanup
    free(buffer);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
}

void stop_audio() {
    // Placeholder for stopping audio playback
    mvprintw(LINES - 1, 0, "Audio stopped.");
    refresh();
}
