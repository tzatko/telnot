#ifndef CLIENT_H
#define CLIENT_H

#include <time.h>
#include "io_handler.h"

typedef struct client_t client_t;

int handle_new_connection(io_handler_t *io_handler, int server_sock);
void handle_client_event(io_handler_t *io_handler, int client_sock, int events);
void cleanup_clients(void);

#endif // CLIENT_H
