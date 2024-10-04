#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/event.h>
#include <errno.h>
#include <string.h>
#include "config.h"
#include "io_handler.h"
#include "server.h"
#include "client.h"

struct io_handler_t {
    int kq;
    struct kevent events[MAX_EVENTS];
};

io_handler_t *create_io_handler(void) {
    io_handler_t *handler = malloc(sizeof(io_handler_t));
    if (!handler) return NULL;

    handler->kq = kqueue();
    if (handler->kq == -1) {
        free(handler);
        return NULL;
    }

    return handler;
}

void destroy_io_handler(io_handler_t *handler) {
    if (handler) {
        close(handler->kq);
        free(handler);
    }
}

int add_to_io_handler(io_handler_t *handler, int fd, int events) {
    struct kevent ev[2];
    int n = 0;

    if (events & EVENT_READ) {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    }
    if (events & EVENT_WRITE) {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    }

    return kevent(handler->kq, ev, n, NULL, 0, NULL) != -1;
}

int remove_from_io_handler(io_handler_t *handler, int fd) {
    struct kevent ev[2];
    EV_SET(&ev[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    EV_SET(&ev[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    return kevent(handler->kq, ev, 2, NULL, 0, NULL) != -1;
}

int handle_events(io_handler_t *handler) {
    struct timespec timeout = {1, 0}; // 1 second timeout
    int nev = kevent(handler->kq, NULL, 0, handler->events, MAX_EVENTS, &timeout);
    
    if (nev == -1) {
        if (errno == EINTR) {
            // Interrupted system call, not an error
            return 0;
        }
        fprintf(stderr, "kevent error: %s\n", strerror(errno));
        return -1;
    }

    if (nev == 0) {
        // Timeout, no events
        return 0;
    }

    for (int i = 0; i < nev; i++) {
        int fd = (int)handler->events[i].ident;
        int events = 0;
        if (handler->events[i].filter == EVFILT_READ) events |= EVENT_READ;
        if (handler->events[i].filter == EVFILT_WRITE) events |= EVENT_WRITE;

        if (fd == get_server_socket()) {
            if (handle_new_connection(handler, fd) < 0) {
                fprintf(stderr, "Error handling new connection\n");
            }
        } else {
            handle_client_event(handler, fd, events);
        }
    }

    return nev;
}

#endif // defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
 

