#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>
#include<stdlib.h>    //malloc, free

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT   27015

// Global array to track thread status (0 = free, 1 = busy)
int thread_status[10] = {0};
pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

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

// Funkcija preuzima komandu u formatu: # word word ... word #
// i vraca niz riječi u komanda[1000][256]
int process_command(char *client_message, char komanda[1000][256], int thread_index) {
    int broj_rijeci = 0;
    
    int len = strlen(client_message);
    if(len >= 3 && client_message[0] == '#' && client_message[len-1] == '#') {
        printf("Thread %d: Processing command format\n", thread_index);
        
        char temp_message[DEFAULT_BUFLEN];
        strcpy(temp_message, client_message);
        
        temp_message[len-1] = '\0';
        char *content = temp_message + 1;
        
        char *token = strtok(content, " \t\n");
        while(token != NULL && broj_rijeci < 1000) {
            strncpy(komanda[broj_rijeci], token, 255);
            komanda[broj_rijeci][255] = '\0';
            broj_rijeci++;
            token = strtok(NULL, " \t\n");
        }
        
        printf("Thread %d: Parsed %d words: ", thread_index, broj_rijeci);
        for(int i = 0; i < broj_rijeci; i++) {
            printf("'%s' ", komanda[i]);
        }
        printf("\n");
        
        return broj_rijeci;
        
    } else {
        printf("Thread %d: Invalid command format. Expected: # word word ... word #\n", thread_index);
        return -1;
    }
}

// FUnkcija za obradu komunikacije sa klijentom
void *connection_handler(void *param)
{
    // Get thread index and socket from parameter
    int *params = (int*)param;
    int thread_index = params[0];
    int sock = params[1];
    
    // Detach the thread so it cleans up automatically
    pthread_detach(pthread_self());
    
    int read_size;
    char client_message[DEFAULT_BUFLEN];

    printf("Thread %d started for client\n", thread_index);

    // Main message receiving loop
    while((read_size = receive_message(sock, client_message, DEFAULT_BUFLEN)) > 0)
    {
        printf("Thread %d received: %s\n", thread_index, client_message);
        
        // Check if client wants to exit
        if(strcmp(client_message, "exit") == 0) {
            printf("Thread %d: Client requested exit\n", thread_index);
            // Send confirmation message before closing
            send_message(sock, "SERVER: Goodbye!");
            break;
        }
        
        // Parse command using the separate function
        char komanda[1000][256];  // Array to store parsed words
        int broj_rijeci = process_command(client_message, komanda, thread_index);
        
        if(broj_rijeci < 0) {
            // Invalid command format
            char error_response[DEFAULT_BUFLEN];
            strcpy(error_response, "SERVER: Invalid format. Use: # command word ... #");
            if(send_message(sock, error_response) < 0) {
                printf("Thread %d: Send failed\n", thread_index);
                break;
            }
        } else {
            // TODO: Process the parsed command here
        
            
            // Send response with parsed information
            char response[DEFAULT_BUFLEN];
            snprintf(response, sizeof(response), "SERVER: Parsed %d words from command", broj_rijeci);
            if(send_message(sock, response) < 0) {
                printf("Thread %d: Send failed\n", thread_index);
                break;
            }
            printf("Thread %d: Sent confirmation for command with %d words\n", thread_index, broj_rijeci);
        }
        
 

        
        
        // Clear the buffer for next message
        memset(client_message, 0, sizeof(client_message));
    }

    // Handle disconnection cases
    if(read_size == 0)
    {
        printf("Client disconnected from thread %d\n", thread_index);
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        printf("Thread %d: recv failed\n", thread_index);
        perror("recv failed");
    }

    // Close socket
    close(sock);
    
    // Mark thread as free
    pthread_mutex_lock(&status_mutex);
    thread_status[thread_index] = 0;
    printf("Thread %d marked as free\n", thread_index);
    pthread_mutex_unlock(&status_mutex);
    
    // Free the parameter array
    free(param);
    
    return NULL;
}

int main(int argc , char *argv[])
{
    pthread_t obradjivac[10]; // Array of threads for handling clients
    
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind done");     

    // Listen
    listen(socket_desc, 3);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Accept and incoming connection
    while(1){
        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if( new_socket < 0)
        {
            perror("Accept failed");
            return 1;
        }
        puts("Connection accepted");

        // Find the next available thread slot
        pthread_mutex_lock(&status_mutex);
        int thread_index = -1;
        for(int i = 0; i < 10; i++) {
            if(thread_status[i] == 0) {
                thread_status[i] = 1; // Mark as busy
                thread_index = i;
                break;
            }
        }
        pthread_mutex_unlock(&status_mutex);

        if(thread_index == -1) {
            printf("All threads are busy, rejecting client\n");
            close(new_socket);
            continue;
        }

        // Create parameter array for thread (index and socket)
        int *params = malloc(2 * sizeof(int));
        params[0] = thread_index;
        params[1] = new_socket;
        
        if (pthread_create(&obradjivac[thread_index], NULL, connection_handler, (void*) params) < 0)
        {
            perror("Could not create thread");
            // Mark thread as free again if creation failed
            pthread_mutex_lock(&status_mutex);
            thread_status[thread_index] = 0;
            pthread_mutex_unlock(&status_mutex);
            free(params);
            return 1; 
        }
        
        printf("Thread %d created for client\n", thread_index);
    }

    return 0;
}