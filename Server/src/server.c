#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>
#include<stdlib.h>    //malloc, free

#include "models/vozilo.h"
#include "models/korisnik.h"
#include "data/data_loader.h"
#include "network/network_utils.h"
#include "commands/command_parser.h"
#include "handlers/client_handler.h"

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT   27015

Korisnik korisnici[100]; // Array to store users
Vozilo vozila[100]; // Array to store vehicles
int broj_korisnika = 0; // Track actual number of users loaded
int broj_vozila = 0; // Track actual number of vehicles loaded

int main(int argc , char *argv[])
{
    pthread_t obradjivac[10]; // Array of threads for handling clients
    
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;

    // Load data form txt files or initialize arrays
    broj_korisnika = load_korisnici_from_file("../podaci/korisnici.txt", korisnici, 100);
    broj_vozila = load_vozila_from_file("../podaci/vozila.txt", vozila, 100);

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