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

typedef struct {
    char name[20];
    char password[20];
}User;

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
    puts("\t - Exit: Izlaz iz aplikacije");
    printf("\n\n");
}

void display_auth_menu()
{
    puts("\n\t\t\t******************************************");
    puts("");
    puts("\t\t\tDobrodosli na stranicu Registar Automobila");
    puts("");
    puts("\t\t\t******************************************\n\n\n");
    puts("Molimo vas da se prijavite ili registrujete:\n");
    puts("Dostupne komande:\n");
    puts("\t - Za prijavu korisnika:");
    puts("\t   # LOGIN [user_name] [password] #");
    puts("\t - Za registraciju novog korisnika:");
    puts("\t   # REGISTRATION [name] [surname] [username] [password] #");
    puts("\t - Exit: Izlaz iz aplikacije");
    printf("\n\n");
}


int main(int argc , char *argv[])
{
    int sock;
    int read_size;
    struct sockaddr_in server;
    
    User user;
    size_t command_count = sizeof(commands) / sizeof(commands[0]);
    int authenticated = 0; // Flag za proveru da li je korisnik ulogovan

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
        // Prikaz menija na osnovu stanja autentifikacije
        if(!authenticated)
        {
            display_auth_menu();
        }
        else
        {
            display_menu(commands, command_count);
        }

        printf("Unesite komandu: ");
        if(fgets(message, DEFAULT_BUFLEN, stdin))
            message[strcspn(message, "\n")] = '\0'; // uklanja novi red koji fgets unese  
        else
        {
            printf("Greska tokom citanja ulaza\n");
            continue;
        }

        // Provera da li korisnik zeli da izadje
        if(strcmp(message, "Exit") == 0)
        {
            printf("Da li stvarno zelite da izadjete iz aplikacije? (Y/N): ");
            char exit_choice = getchar();
            while (getchar() != '\n'); // Čisti ulazni bafer
            
            if(exit_choice == 'Y' || exit_choice == 'y')
            {
                printf("Zatvaranje veze sa serverom...\n");
                break;
            }
            else
            {
                continue;
            }
        }

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
            // //Slanje potvrde serveru da je porukla stigla do klijenta
            // if( send(sock , "PRIMLJENO" , strlen("PRIMLJENO"), 0) < 0)
            // {
            //     puts("Send failed");
            //     return 1;
            // }

            printf("Primljena poruka od strane servera:\n");
            printf("%s\n\n", message_from_server);

            // Handle specific LOGIN ERROR responses
            if(strcmp(message_from_server, "# LOGIN ERROR 101 #") == 0)
            {
                printf("GRESKA: Uneli ste pogresnu lozinku!\n\n");
            }
            else if(strcmp(message_from_server, "# LOGIN ERROR 102 #") == 0)
            {
                printf("GRESKA: Uneli ste pogresno korisnicko ime!\n\n");
            }
            else if(strncmp(message_from_server, "# LOGIN SUCCESS", 15) == 0)
            {
                printf("Uspesno ste se prijavili!\n\n");
                authenticated = 1;
            }
            // Handle REGISTRATION ERROR responses
            else if(strcmp(message_from_server, "# REGISTRATION 201 #") == 0)
            {
                printf("GRESKA: Korisnik vec postoji!\n\n");
            }
            else if(strcmp(message_from_server, "# REGISTRATION 202 #") == 0)
            {
                printf("GRESKA: Korisnicko ime je zauzeto!\n\n");
            }
            else if(strcmp(message_from_server, "# REGISTRATION SUCCESS #") == 0)
            {
                printf("Uspesno ste se registrovali!\n\n");
                authenticated = 1;
            }

            if(authenticated)
            {
                printf("Da li zelite da unesete drugu komandu? (Y/N): ");
            }
            else
            {
                printf("Da li zelite da se prijavite/registrujete ponovo? (Y/N): ");
            }
            
            char ch = getchar();
            while (getchar() != '\n'); // Čisti ulazni bafer
            if(ch == 'Y' || ch == 'y')
            {
                continue;
            }
            else
            {
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

