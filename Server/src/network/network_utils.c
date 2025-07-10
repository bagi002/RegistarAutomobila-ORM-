#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "network_utils.h"

// funkcija za primanje poruke od klijenta
int receive_message(int sock, char *buffer, int buffer_size) {
    int read_size = recv(sock, buffer, buffer_size - 1, 0);
    if (read_size > 0) {
        buffer[read_size] = '\0'; // Null terminate the message
    }
    return read_size;
}

// Funkcija za slanje poruke klijentu
int send_message(int sock, const char *message) {
    return send(sock, message, strlen(message), 0);
}