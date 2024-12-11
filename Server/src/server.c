/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2024/25
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP/IP server
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT   27015

static int j = 1;


void* communication(void *param)
{
    int read_size;
    int client_sock = *(int*)param;
    char client_message[DEFAULT_BUFLEN];

    while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Primljenih bajtova %d\n", read_size);
        puts("Primljena poruka:");
        client_message[read_size] = '\0';
        printf("%s\n", client_message); 
        printf("\n");
        //Slanje poruke nazad klijentu
        /*if(send(client_sock, client_message, read_size, 0) < 0)
        {
            puts("Send back to client failed");
            return 1;
        }
        */
    }
    
    if(read_size == 0)
    {
     
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    close(client_sock);
    return 0;
}
int main(int argc , char *argv[])
{
    
    int socket_desc;
    int client_sock , c , read_size;
    struct sockaddr_in server , client;
    pthread_t client_thread[3];
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    
    while(1)
    {
        
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }
        printf("Uspesno kontektovan klijent %d\n", j);
        
        pthread_create(&client_thread[j], NULL, communication, (void*)&client_sock);
        j++;
    }
    
   // close(client_sock);
   // close(socket_desc);

    return 0;
}

