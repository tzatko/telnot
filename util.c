#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stddef.h>
#include "config.h"
#include "util.h"

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

char *get_random_file(void) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char *filepath = NULL;
    int count = 0;

    dir = opendir(TEXT_DIR);
    if (dir == NULL) {
        perror("opendir failed");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[MAX_PATH_LENGTH];
        int path_length = snprintf(fullpath, sizeof(fullpath), "%s/%s", TEXT_DIR, entry->d_name);
        
        if (path_length < 0 || (size_t)path_length >= sizeof(fullpath)) {
            fprintf(stderr, "Path too long or encoding error: %s/%s\n", TEXT_DIR, entry->d_name);
            continue;
        }
        
        if (stat(fullpath, &statbuf) == -1) {
            perror("stat failed");
            continue;
        }
        
        if (S_ISREG(statbuf.st_mode)) {
            count++;
        }
    }

    if (count == 0) {
        closedir(dir);
        return NULL;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand(ts.tv_nsec); // Use nanoseconds for better randomness
    int random_index = rand() % count;
    rewinddir(dir);

    int current_index = 0;
    while ((entry = readdir(dir)) != NULL) {
        char fullpath[MAX_PATH_LENGTH];
        int path_length = snprintf(fullpath, sizeof(fullpath), "%s/%s", TEXT_DIR, entry->d_name);
        
        if (path_length < 0 || (size_t)path_length >= sizeof(fullpath)) {
            continue;
        }
        
        if (stat(fullpath, &statbuf) == -1) {
            continue;
        }
        
        if (S_ISREG(statbuf.st_mode)) {
            if (current_index == random_index) {
                filepath = strdup(fullpath);
                break;
            }
            current_index++;
        }
    }

    closedir(dir);
    return filepath;
}

void log_connection(const char *client_ip, time_t start_time, time_t end_time, 
                    size_t bytes_transferred, int connection_closed_by_server, 
                    const char *filename) {
    pthread_mutex_lock(&log_mutex);
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("fopen log file failed");
        pthread_mutex_unlock(&log_mutex);
        return;
    }

    char start_time_str[20];
    strftime(start_time_str, sizeof(start_time_str), "%Y-%m-%d %H:%M:%S", localtime(&start_time));

    double duration = difftime(end_time, start_time);

    fprintf(log_file, "SRC: %s | Time: %s | Duration: %.0fs | File: %s | Bytes: %zu | Closed by: %s\n",
            client_ip, start_time_str, duration, filename ? filename : "N/A", bytes_transferred,
            connection_closed_by_server ? "Server" : "Client");
    
    fclose(log_file);
    pthread_mutex_unlock(&log_mutex);
}

void get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen) {
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, maxlen);
            break;
        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, maxlen);
            break;
        default:
            strncpy(s, "Unknown AF", maxlen);
            s[maxlen - 1] = '\0';  // Ensure null-termination
    }
}

