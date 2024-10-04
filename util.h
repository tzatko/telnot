#ifndef UTIL_H
#define UTIL_H

#include <time.h>
#include <sys/socket.h>

char *get_random_file(void);
void log_connection(const char *client_ip, time_t start_time, time_t end_time, 
                    size_t bytes_transferred, int connection_closed_by_server, 
                    const char *filename);
void get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);

#endif // UTIL_H
