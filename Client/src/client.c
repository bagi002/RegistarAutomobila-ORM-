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
#include "menu.h"

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT   27015

// Uncomment the line below to enable debug output
#define DEBUG


int main(int argc , char *argv[])
{
    int sock;
    int read_size;
    struct sockaddr_in server;
    
    User user;
    // Inicijalizacija strukture
    memset(&user, 0, sizeof(User));
    
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
            display_menu(&user);
        }

        if(fgets(message, DEFAULT_BUFLEN, stdin))
            message[strcspn(message, "\n")] = '\0'; // uklanja novi red koji fgets unese  
        else
        {
            printf("Greska tokom citanja ulaza\n");
            continue;
        }

        if(!authenticated)
        {
            if(strcmp(message, "1") == 0)
            {
                display_login_form();
                char temp_name[256];
                char temp_psw[256];
                printf("Korisničko ime: ");
                if(fgets(temp_name, sizeof(temp_name), stdin))
                    temp_name[strcspn(temp_name, "\n")] = '\0';
                printf("Lozinka: ");
                if(fgets(temp_psw, sizeof(temp_psw), stdin))
                    temp_psw[strcspn(temp_psw, "\n")] = '\0';
                puts("\nPodaci su uspešno uneti!");
                snprintf(message, DEFAULT_BUFLEN, "# LOGIN [%s] [%s] #", temp_name, temp_psw);
                #ifdef DEBUG
                    printf("# LOGIN [%s] [%s] #", temp_name, temp_psw);
                #endif
                
                
            }
            else if(strcmp(message, "2") == 0)
            {
                display_registration_form(&user);
                snprintf(message, DEFAULT_BUFLEN, "# REGISTRATION [%s] [%s] [%s] [%s] #", 
                         user.first_name, user.last_name, user.name, user.password);
                #ifdef DEBUG
                printf("# REGISTRATION [%s] [%s] [%s] [%s] #", 
                       user.first_name, user.last_name, user.name, user.password);
                #endif
            }
            else if(strcmp(message, "3") == 0)
            {
                printf("Zatvaranje aplikacije...\n");
                break;
            }
            else
            {
                printf("Nepoznata opcija. Molimo vas da unesete 1, 2 ili 3.\n");
                continue;
            }
        }

        // Ako je korisnik autentifikovan, obrađuj komande iz menija
        else
        {
            int choice = atoi(message);
            switch(choice)
            {
                case 1:
                    // Logout
                    snprintf(message, DEFAULT_BUFLEN, "# LOGOUT [%d] #", user.id);
                #ifdef DEBUG
                    printf("# LOGOUT [%d] #", user.id);
                #endif
                break;
                
                case 2:
                    // Search
                    snprintf(message, DEFAULT_BUFLEN, "Search");
#ifdef DEBUG
                    printf("Odabrana komanda: Search\n");
#endif
                    break;
                case 3:
                    // SearchAll
                    snprintf(message, DEFAULT_BUFLEN, "SearchAll");
#ifdef DEBUG
                    printf("Odabrana komanda: SearchAll\n");
#endif
                    break;
                case 4:
                    // CheckStatus
                    snprintf(message, DEFAULT_BUFLEN, "CheckStatus");
#ifdef DEBUG
                    printf("Odabrana komanda: CheckStatus\n");
#endif
                    break;
                case 5:
                    // Reserve
                    snprintf(message, DEFAULT_BUFLEN, "Reserve");
#ifdef DEBUG
                    printf("Odabrana komanda: Reserve\n");
#endif
                    break;
                case 6:
                    // Exit
                    printf("Zatvaranje aplikacije...\n");
                    break;
                default:
                    printf("Nepoznata opcija. Molimo vas da unesete broj između 1 i 6.\n");
                    continue;
            }
            
            if(choice == 6)
            {
                break;
            }
        }

        //Slanje komande serveru
        if( send(sock , message , strlen(message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        printf("\nKomanda %s je uspesno poslata serveru.\n\n\n", message);

        //Prijem poruke od servera
        if(read_size = recv(sock, message_from_server, DEFAULT_BUFLEN, 0) > 0)
        {
            #ifdef DEBUG
                printf("Primljena poruka od strane servera:\n");
                printf("%s\n\n", message_from_server);
            #endif
            
            if(strcmp(message_from_server, "# LOGIN ERROR 101 #") == 0)
            {
                printf("GRESKA: Uneli ste pogresnu lozinku!\n\n");
            }
            else if(strcmp(message_from_server, "# LOGIN ERROR 102 #") == 0)
            {
                printf("GRESKA: Uneli ste pogresno korisnicko ime!\n\n");
            }
            else if(strncmp(message_from_server, "# LOGIN SUCCES", 14) == 0)
            {
                printf("Uspesno ste se prijavili!\n\n");
                authenticated = 1;
                
                // Parsiranje poruke za izvlačenje ID-a, imena i prezimena
                // Format: "# LOGIN SUCCES ID [ime] [prezime] #"
                char temp_message[DEFAULT_BUFLEN];
                strncpy(temp_message, message_from_server, DEFAULT_BUFLEN - 1);
                temp_message[DEFAULT_BUFLEN - 1] = '\0';
                
                // Pronađi ID između "SUCCES" i prvi "["
                char *id_start = strstr(temp_message, "SUCCES");
                if(id_start != NULL) {
                    id_start += 7; // Preskoči "SUCCES "
                    char *id_end = strchr(id_start, '[');
                    if(id_end != NULL) {
                        *id_end = '\0'; // Privremeno završi string
                        // Ukloni razmake
                        while(*id_start == ' ') id_start++;
                        id_end--;
                        while(id_end > id_start && *id_end == ' ') {
                            *id_end = '\0';
                            id_end--;
                        }
                        user.id = atoi(id_start);
                        
                        #ifdef DEBUG
                            printf("Korisnikov ID: %d\n", user.id);
                        #endif
                        
                        // Vrati originalni string za dalje parsiranje
                        strncpy(temp_message, message_from_server, DEFAULT_BUFLEN - 1);
                        temp_message[DEFAULT_BUFLEN - 1] = '\0';
                    }
                }
                
                char *start = strchr(temp_message, '[');
                if(start != NULL) {
                    start++; // Preskoči '['
                    char *end = strchr(start, ']');
                    if(end != NULL) {
                        *end = '\0'; // Završi prvi string
                        strncpy(user.first_name, start, sizeof(user.first_name) - 1);
                        user.first_name[sizeof(user.first_name) - 1] = '\0';
                        
                        // Traži drugu zagradu
                        start = strchr(end + 1, '[');
                        if(start != NULL) {
                            start++; // Preskoči '['
                            end = strchr(start, ']');
                            if(end != NULL) {
                                *end = '\0'; // Završi drugi string
                                strncpy(user.last_name, start, sizeof(user.last_name) - 1);
                                user.last_name[sizeof(user.last_name) - 1] = '\0';
                            }
                        }
                    }
                }
                
                #ifdef DEBUG
                    printf("Korisnikovo ime: %s\n", user.first_name);
                    printf("Korisnikovo prezime: %s\n", user.last_name);
                #endif
                
                continue; // Prikaži glavni meni nakon uspešne prijave
            }
            else if(strcmp(message_from_server, "# REGISTRATION 201 #") == 0)
            {
                printf("GRESKA: Korisnik vec postoji!\n\n");
                memset(&user, 0, sizeof(User));
            }
            else if(strcmp(message_from_server, "# REGISTRATION 202 #") == 0)
            {
                printf("GRESKA: Korisnicko ime je zauzeto!\n\n");
                memset(&user, 0, sizeof(User));
            }
            else if(strncmp(message_from_server, "# REGISTRATION SUCCES", 21) == 0)
            {
                printf("Uspesno ste se registrovali!\n\n");
                authenticated = 1;
                
                // Parsiranje poruke za izvlačenje ID-a
                // Format: "# REGISTRATION SUCCES ID #"
                char temp_message[DEFAULT_BUFLEN];
                strncpy(temp_message, message_from_server, DEFAULT_BUFLEN - 1);
                temp_message[DEFAULT_BUFLEN - 1] = '\0';
                
                // Pronađi ID između "SUCCES" i "#"
                char *start = strstr(temp_message, "SUCCES");
                if(start != NULL) {
                    start += 7; // Preskoči "SUCCES "
                    char *end = strchr(start, '#');
                    if(end != NULL) {
                        *end = '\0'; // Završi string
                        // Ukloni razmake
                        while(*start == ' ') start++;
                        end--;
                        while(end > start && *end == ' ') {
                            *end = '\0';
                            end--;
                        }
                        user.id = atoi(start);
                        
                        #ifdef DEBUG
                            printf("Korisnikov ID: %d\n", user.id);
                        #endif
                    }
                }
                
                continue; // Prikaži glavni meni nakon uspešne registracije
            }
            else if(authenticated)
            {
                printf("Odgovor servera:\n%s\n\n", message_from_server);
                
                if(strcmp(message_from_server, "# LOGOUT SUCCES #") == 0)
                {
                    printf("Uspešno ste se odjavili!\n\n");
                    authenticated = 0;
                    
                    // Obriši podatke o korisniku
                    memset(user.first_name, 0, sizeof(user.first_name));
                    memset(user.last_name, 0, sizeof(user.last_name));
                    //memset(user.name, 0, sizeof(user.name));
                    
                    continue;
                }
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

