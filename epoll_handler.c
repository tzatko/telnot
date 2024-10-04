#ifdef __linux__

#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "config.h"
#include "io_handler.h"
#include "server.h"
#include "client.h"

struct io_handler_t {
    int epoll_fd;
    struct epoll_event events[MAX_EVENTS];
};

io_handler_t *create_io_handler() {
    io_handler_t *handler = malloc(sizeof(io_handler_t));
    if (!handler) return NULL;

    handler->epoll_fd = epoll_create1(0);
    if (handler->epoll_fd == -1) {
        free(handler);
        return NULL;
    }

    return handler;
}

void destroy_io_handler(io_handler_t *handler) {
    if (handler) {
        close(handler->epoll_fd);
        free(handler);
    }
}

int add_to_io_handler(io_handler_t *handler, int fd, int events) {
    struct epoll_event ev;
    ev.events = 0;
    if (events & EVENT_READ) ev.events |= EPOLLIN;
    if (events & EVENT_WRITE) ev.events |= EPOLLOUT;
    ev.data.fd = fd;

    return epoll_ctl(handler->epoll_fd, EPOLL_CTL_ADD, fd, &ev) == 0;
}

int remove_from_io_handler(io_handler_t *handler, int fd) {
    return epoll_ctl(handler->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == 0;
}

int handle_events(io_handler_t *handler) {
    int nfds = epoll_wait(handler->epoll_fd, handler->events, MAX_EVENTS, -1);
    if (nfds == -1) return -1;

    for (int i = 0; i < nfds; i++) {
        int fd = handler->events[i].data.fd;
        int events = 0;
        if (handler->events[i].events & EPOLLIN) events |= EVENT_READ;
        if (handler->events[i].events & EPOLLOUT) events |= EVENT_WRITE;

        if (fd == get_server_socket()) {
            handle_new_connection(handler, fd);
        } else {
            handle_client_event(handler, fd, events);
        }
    }

    return 0;
}

#endif // __linux__
