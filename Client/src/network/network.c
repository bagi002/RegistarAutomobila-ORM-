#include "../menu/menu.h"

int create_socket_connection(void)
{
    int sock;
    struct sockaddr_in server;
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return -1;
    }
    puts("Socket created");
    
    // Setup server address
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        close(sock);
        return -1;
    }
    
    puts("Connected\n");
    return sock;
}