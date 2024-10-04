CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L
LDFLAGS = -pthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS += -D__linux__
else
    CFLAGS += -D__APPLE__
endif

SRCS = main.c server.c client.c util.c
ifeq ($(UNAME_S),Linux)
    SRCS += epoll_handler.c
else
    SRCS += kqueue_handler.c
endif

OBJS = $(SRCS:.c=.o)
TARGET = fake_telnet_server

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
