#ifndef MENU_H
#define MENU_H

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
    char first_name[20];
    char last_name[20];
    char name[20];
    char password[20];
    unsigned id;
} User;

typedef struct{
    unsigned id;                    // ID vozila
    char manufacturer[20];          // Proizvođač
    char model[20];                 // Model
    int manufacturer_year;          // Godina proizvodnje
    char color[20];                 // Boja
    char plates[20];                // Registarske tablice
    unsigned status;                // Status (0 = dostupno, 1 = rezervisano)
    unsigned reserved_by_user_id;   // ID korisnika koji je rezervisao (0 ako nije rezervisano)
} Vehicle;


void display_menu(const User* user);
void display_auth_menu();
void display_login_form();
void display_registration_form(User* user);

int create_socket_connection(void);
int handle_authentication_menu(int sock, User *user, char *message, char *server_response);
int handle_main_menu(int sock, User *user, char *message, char *server_response);
int process_server_response(char *server_response, User *user, int *authenticated);
void parse_login_success(char *message, User *user);
void parse_registration_success(char *message, User *user);
void cleanup_user_data(User *user);

#endif // MENU_H
