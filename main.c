#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "config.h"
#include "server.h"
#include "io_handler.h"

static volatile int keep_running = 1;

void handle_signal(int sig) {
    (void)sig; // Mark as unused
    keep_running = 0;
}

int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    io_handler_t *io_handler = create_io_handler();
    if (!io_handler) {
        fprintf(stderr, "Failed to create I/O handler\n");
        return EXIT_FAILURE;
    }

    if (!setup_server(io_handler)) {
        fprintf(stderr, "Failed to set up server\n");
        destroy_io_handler(io_handler);
        return EXIT_FAILURE;
    }

    printf("Fake Telnet service listening on port %s...\n", PORT);

    while (keep_running) {
        int result = handle_events(io_handler);
        if (result < 0) {
            fprintf(stderr, "Error handling events: %s\n", strerror(errno));
            break;
        }
        // Removed the "No events handled" message
    }

    cleanup_server();
    destroy_io_handler(io_handler);
    printf("Server shut down gracefully\n");
    return EXIT_SUCCESS;
}

