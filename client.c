#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include "config.h"
#include "client.h"
#include "util.h"

struct client_t {
    int sock;
    char ip[INET6_ADDRSTRLEN];
    time_t start_time;
    size_t bytes_transferred;
    char *file_path;
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t buffer_pos;
    size_t buffer_size;
};

static client_t *clients[MAX_CONNECTIONS] = {NULL};
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static void apply_random_delay(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand(ts.tv_nsec);
    usleep(MIN_DELAY + rand() % (MAX_DELAY - MIN_DELAY + 1));
}

static void close_client(io_handler_t *io_handler, client_t *client) {
    if (!client) return;

    remove_from_io_handler(io_handler, client->sock);
    close(client->sock);

    if (client->file) {
        fclose(client->file);
    }
    
    time_t end_time = time(NULL);
    log_connection(client->ip, client->start_time, end_time, 
                   client->bytes_transferred, 1, client->file_path);

    free(client->file_path);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clients[i] == client) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    free(client);
}

int handle_new_connection(io_handler_t *io_handler, int server_sock) {
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    
    if (client_sock == -1) {
        perror("accept failed");
        return -1;
    }

    if (fcntl(client_sock, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl failed");
        close(client_sock);
        return -1;
    }

    client_t *client = calloc(1, sizeof(client_t));
    if (!client) {
        perror("calloc failed");
        close(client_sock);
        return -1;
    }

    client->sock = client_sock;
    client->start_time = time(NULL);
    get_ip_str((struct sockaddr *)&client_addr, client->ip, sizeof(client->ip));

    client->file_path = get_random_file();
    if (client->file_path) {
        client->file = fopen(client->file_path, "r");
        if (!client->file) {
            perror("fopen failed");
            free(client->file_path);
            free(client);
            close(client_sock);
            return -1;
        }
    } else {
        const char *error_message = "No files available. Please add some text files.\n";
        strncpy(client->buffer, error_message, sizeof(client->buffer));
        client->buffer_size = strlen(error_message);
    }

    pthread_mutex_lock(&clients_mutex);
    int slot = -1;
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clients[i] == NULL) {
            clients[i] = client;
            slot = i;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (slot == -1) {
        fprintf(stderr, "Max connections reached\n");
        if (client->file) fclose(client->file);
        free(client->file_path);
        free(client);
        close(client_sock);
        return -1;
    }

    if (!add_to_io_handler(io_handler, client_sock, EVENT_READ | EVENT_WRITE)) {
        close_client(io_handler, client);
        return -1;
    }

    return 0;
}

void handle_client_event(io_handler_t *io_handler, int client_sock, int events) {
    client_t *client = NULL;
    
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clients[i] && clients[i]->sock == client_sock) {
            client = clients[i];
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (!client) return;

    if (events & EVENT_READ) {
        char buf[1];
        ssize_t recv_result = recv(client_sock, buf, 1, MSG_PEEK);
        if (recv_result <= 0) {
            if (recv_result == 0 || (recv_result == -1 && errno != EAGAIN && errno != EWOULDBLOCK)) {
                close_client(io_handler, client);
                return;
            }
        }
    }

    if (events & EVENT_WRITE) {
        if (client->buffer_pos < client->buffer_size) {
            ssize_t sent = send(client_sock, client->buffer + client->buffer_pos, 1, MSG_NOSIGNAL);
            if (sent > 0) {
                client->buffer_pos += sent;
                client->bytes_transferred += sent;
                apply_random_delay();
            } else if (sent == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
                close_client(io_handler, client);
                return;
            }
        } else if (client->file) {
            size_t bytes_read = fread(client->buffer, 1, sizeof(client->buffer), client->file);
            if (bytes_read > 0) {
                client->buffer_size = bytes_read;
                client->buffer_pos = 0;
            } else {
                if (feof(client->file)) {
                    close_client(io_handler, client);
                } else {
                    perror("fread failed");
                    close_client(io_handler, client);
                }
                return;
            }
        } else {
            close_client(io_handler, client);
            return;
        }
    }
}

void cleanup_clients(void) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clients[i]) {
            close_client(NULL, clients[i]);
            clients[i] = NULL;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

