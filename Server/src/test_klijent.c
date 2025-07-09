#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT 27015

// Global variables for thread communication
int server_socket;
int keep_running = 1;

/**
 * Function to send a message to server
 * @param sock - server socket descriptor
 * @param message - message to send
 * @return number of bytes sent, -1 on error
 */
int send_message(int sock, const char *message) {
    return send(sock, message, strlen(message), 0);
}

/**
 * Function to receive a message from server
 * @param sock - server socket descriptor
 * @param buffer - buffer to store received message
 * @param buffer_size - size of the buffer
 * @return number of bytes received, 0 if server disconnected, -1 on error
 */
int receive_message(int sock, char *buffer, int buffer_size) {
    int read_size = recv(sock, buffer, buffer_size - 1, 0);
    if (read_size > 0) {
        buffer[read_size] = '\0'; // Null terminate the message
    }
    return read_size;
}

/**
 * Thread function to continuously receive messages from server
 * @param param - socket descriptor as void pointer
 * @return NULL when thread terminates
 */
void *receive_thread(void *param) {
    int sock = *(int*)param;
    char server_reply[DEFAULT_BUFLEN];
    int reply_size;
    
    printf("[RECEIVER] Started listening for server responses...\n");
    
    while(keep_running) {
        reply_size = receive_message(sock, server_reply, sizeof(server_reply));
        
        if(reply_size > 0) {
            printf("\n[SERVER RESPONSE]: %s\n", server_reply);
            printf("Enter command: ");
            fflush(stdout); // Force output to appear immediately
        }
        else if(reply_size == 0) {
            printf("\n[RECEIVER] Server disconnected\n");
            keep_running = 0;
            break;
        }
        else {
            if(keep_running) { // Only show error if we're still supposed to be running
                printf("\n[RECEIVER] Error receiving message\n");
            }
            break;
        }
        
        memset(server_reply, 0, sizeof(server_reply));
    }
    
    printf("[RECEIVER] Thread terminated\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    char message[DEFAULT_BUFLEN];
    pthread_t receiver_thread;
    
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Could not create socket\n");
        return 1;
    }
    printf("Socket created\n");
    
    // Configure server address
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    
    // Connect to remote server
    if (connect(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connect failed");
        return 1;
    }
    printf("Connected to server\n");
    printf("===========================================\n");
    printf("You can now send commands continuously!\n");
    printf("Type 'exit' to quit the application\n");
    printf("===========================================\n");
    
    // Create receiver thread
    if(pthread_create(&receiver_thread, NULL, receive_thread, (void*)&server_socket) < 0) {
        perror("Could not create receiver thread");
        return 1;
    }
    
    // Main input loop - continuously accept commands
    while(keep_running)
    {
        printf("Enter command: ");
        fflush(stdout);
        
        if(fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }
        
        // Remove newline character from fgets
        message[strcspn(message, "\n")] = 0;
        
        // Check if user wants to exit
        if(strcmp(message, "exit") == 0) {
            printf("Sending exit command and waiting for server response...\n");
            
            // Send exit message to server
            if(send_message(server_socket, message) < 0) {
                printf("Send failed\n");
                break;
            }
            printf("[SENT]: %s\n", message);
            
            // Wait for server response before exiting
            char server_reply[DEFAULT_BUFLEN];
            int reply_size = receive_message(server_socket, server_reply, sizeof(server_reply));
            
            if(reply_size > 0) {
                printf("[SERVER FINAL RESPONSE]: %s\n", server_reply);
            } else if(reply_size == 0) {
                printf("Server disconnected\n");
            } else {
                printf("Error receiving final response\n");
            }
            
            printf("Exiting...\n");
            keep_running = 0;
            break;
        }
        
        // Send message to server immediately
        if(send_message(server_socket, message) < 0)
        {
            printf("Send failed\n");
            break;
        }
        
        printf("[SENT]: %s\n", message);
        
        // Clear message buffer
        memset(message, 0, sizeof(message));
    }
    
    // Cleanup
    keep_running = 0;
    
    // Wait for receiver thread to finish
    pthread_join(receiver_thread, NULL);
    
    close(server_socket);
    printf("Connection closed\n");
    return 0;
}