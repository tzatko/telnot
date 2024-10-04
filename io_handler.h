#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#define EVENT_READ  1
#define EVENT_WRITE 2

typedef struct io_handler_t io_handler_t;

io_handler_t *create_io_handler(void);
void destroy_io_handler(io_handler_t *handler);
int add_to_io_handler(io_handler_t *handler, int fd, int events);
int remove_from_io_handler(io_handler_t *handler, int fd);
int handle_events(io_handler_t *handler);

#endif // IO_HANDLER_H
