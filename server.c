#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "config.h"
#include "server.h"
#include "client.h"
#include "io_handler.h"

static int server_sock = -1;

static int create_and_bind(void) {
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        server_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_sock == -1) continue;

        int opt = 1;
        if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            perror("setsockopt failed");
            close(server_sock);
            continue;
        }

        if (bind(server_sock, p->ai_addr, p->ai_addrlen) == 0) break;

        close(server_sock);
    }

    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "Failed to bind to any address\n");
        return -1;
    }

    return server_sock;
}

int setup_server(io_handler_t *io_handler) {
    server_sock = create_and_bind();
    if (server_sock == -1) return 0;

    if (listen(server_sock, BACKLOG) == -1) {
        perror("listen failed");
        close(server_sock);
        return 0;
    }

    if (fcntl(server_sock, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl failed");
        close(server_sock);
        return 0;
    }

    if (!add_to_io_handler(io_handler, server_sock, EVENT_READ)) {
        close(server_sock);
        return 0;
    }

    return 1;
}

void cleanup_server(void) {
    if (server_sock != -1) {
        close(server_sock);
        server_sock = -1;
    }
    cleanup_clients();
}

int get_server_socket(void) {
    return server_sock;
}

