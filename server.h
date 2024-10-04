#ifndef SERVER_H
#define SERVER_H

#include "io_handler.h"

int setup_server(io_handler_t *io_handler);
void cleanup_server(void);
int get_server_socket(void);

#endif // SERVER_H
