/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2024/25
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           Registar Automobila TCP/IP
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <fcntl.h>      //for open
#include <unistd.h>     //for close
#include <stdlib.h>     //malloc
#include <string.h>     //strcmp

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT   27015

typedef struct {
    const char* name;   // Ime komande
    const char* description; //Opis komande
} Command;

const Command commands[] = {
    {"Login", "Prijava/Registracija korisnika"},
    {"Logout", "Odjava korisnika"},
    {"Search", "Pretraga dostupnih automobila u elektronskom registru"},
    {"SearchAll", "Pretraga svih automobila u elektronskom registru"},
    {"Search[id:ID][manufacturer:MANUFACTURER][carname:CARNAME][year:YEAR]", "Pretraga za zadate kriterijume (sve ili pojedinacno navedene)"},
    {"CheckStatus", "Provera rezervisanih automobila prijavljenog korisnika"},
    {"Reserve[id:ID]", "Rezervacija automobila od strane prijavljenog korisnika po ID-u automobila"}
};

void display_menu(const Command* commands, size_t command_counter)
{
    puts("\n\t\t\t******************************************");
    puts("");
    puts("\t\t\tDobrodosli na stranicu Registar Automobila");
    puts("");
    puts("\t\t\t******************************************\n\n\n");
    puts("Dostupne komande:\n");
    for(size_t i = 0; i < command_counter; i++)
    {
        printf("\t - %s: %s\n", commands[i].name, commands[i].description);
    }
    printf("\n\n");
}

void execute_command(const char* command, const Command* commands, size_t command_count)
{
    for(size_t i = 0; i < command_count; i++)
    {
        if(strcmp(commands[i].name, command) == 0)
        {
            printf("Izvrsavanje komande: %s\n", commands[i].name);
            return;
        }
    }
}

int main(int argc , char *argv[])
{
    int sock;
    int read_size;
    struct sockaddr_in server;
    
    size_t command_count = sizeof(commands) / sizeof(commands[0]);

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    char* message = (char*) malloc(DEFAULT_BUFLEN * sizeof(char));
    if(message == NULL)
    {
        printf("Memory not allocated.\n");
        exit(0);
    }

    char* message_from_server = (char*) malloc(DEFAULT_BUFLEN * sizeof(char));
    if(message_from_server == NULL)
    {
        printf("Memory not allocated.\n");
        exit(0);
    }

    while(1)
    {
        display_menu(commands, command_count);
        printf("Unesite komandu: ");
        if(fgets(message, DEFAULT_BUFLEN, stdin))
            message[strcspn(message, "\n")] = '\0'; // uklanja novi red koji fgets unese  
        else
        {
            printf("Greska tokom citanja ulaza\n");
            continue;
        }

        execute_command(message, commands, command_count);

        //Slanje komande serveru
        if( send(sock , message , strlen(message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        printf("\nKomanda je uspesno poslata serveru.\n\n\n");

        //Prijem poruke od servera
        if(read_size = recv(sock, message_from_server, DEFAULT_BUFLEN, 0) > 0)
        {
            //Slanje potvrde serveru da je porukla stigla do klijenta
            if( send(sock , "PRIMLJENO" , strlen("PRIMLJENO"), 0) < 0)
            {
                puts("Send failed");
                return 1;
            }

            printf("Primljena poruka od strane servera:\n");
            printf("%s\n\n", message_from_server);
            printf("Da li zelite da unesete drugu komandu? (Y/N)");
            char ch = getchar();
            while (getchar() != '\n'); // Čisti ulazni bafer
            if(ch == 'Y' || ch == 'y')
            {
                free(message_from_server);
                continue;
            }
            else
            {
                free(message_from_server);
                break;
            }
        }
        if(read_size == 0)
        {
            fflush(stdout);
        }
        else if(read_size == -1)
        {
            perror("recv failed");
        }
    }
    printf("Hvala Vam sto ste posetili nasu stranicu :)\n");
    free(message);
    close(sock);
    
    return 0;
}

