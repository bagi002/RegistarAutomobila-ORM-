#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <sys/socket.h>

// Function declarations for network operations
int receive_message(int sock, char *buffer, int buffer_size);
int send_message(int sock, const char *message);

#endif