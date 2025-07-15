#include "menu.h"
#include "parser.h"


// Function to handle user navigation after displaying vehicles
int handle_vehicle_display_navigation() {
    char choice[10];
    
    while (1) {
        printf("\n=== OPCIJE ===\n");
        printf("1. Povratak na glavni meni\n");
        printf("Unesite opciju: ");
        
        if (fgets(choice, sizeof(choice), stdin)) {
            choice[strcspn(choice, "\n")] = '\0';
            
            if (strcmp(choice, "1") == 0) {
                printf("Povratak na glavni meni...\n\n");
                return 1;
            } else {
                printf("Nepoznata opcija. Molimo pokušajte ponovo.\n");
                // Loop continues, asking for input again
            }
        } else {
            printf("Greška pri čitanju unosa. Molimo pokušajte ponovo.\n");
            // Loop continues, asking for input again
        }
    }
}

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

int handle_authentication_menu(int sock, User *user, char *message, char *server_response)
{
    display_auth_menu();
    
    if (!fgets(message, DEFAULT_BUFLEN, stdin)) {
        printf("Greska tokom citanja ulaza\n");
        return 0;
    }
    message[strcspn(message, "\n")] = '\0';
    
    if (strcmp(message, "1") == 0) {
        // Login
        display_login_form();
        char temp_name[256], temp_psw[256];
        
        printf("Korisničko ime: ");
        if (fgets(temp_name, sizeof(temp_name), stdin))
            temp_name[strcspn(temp_name, "\n")] = '\0';
        
        printf("Lozinka: ");
        if (fgets(temp_psw, sizeof(temp_psw), stdin))
            temp_psw[strcspn(temp_psw, "\n")] = '\0';
        
        puts("\nPodaci su uspešno uneti!");
        snprintf(message, DEFAULT_BUFLEN, "# LOGIN [%s] [%s] #", temp_name, temp_psw);
        
        #ifdef DEBUG
            printf("Šalje se: %s\n", message);
        #endif
        
    } else if (strcmp(message, "2") == 0) {
        // Registration
        display_registration_form(user);
        snprintf(message, DEFAULT_BUFLEN, "# REGISTRATION [%s] [%s] [%s] [%s] #", 
                 user->first_name, user->last_name, user->name, user->password);
        
        #ifdef DEBUG
            printf("Šalje se: %s\n", message);
        #endif
        
    } else if (strcmp(message, "3") == 0) {
        // Exit
        printf("Zatvaranje aplikacije...\n");
        return -1;
    } else {
        printf("Nepoznata opcija. Molimo vas da unesete 1, 2 ili 3.\n");
        return 0;
    }
    
    // Send message to server
    if (send(sock, message, strlen(message), 0) < 0) {
        puts("Send failed");
        return 0;
    }
    printf("\nKomanda je uspešno poslata serveru.\n\n");
    
    // Receive response from server
    int read_size = recv(sock, server_response, DEFAULT_BUFLEN - 1, 0);
    if (read_size > 0) {
        server_response[read_size] = '\0';
        return process_server_response(server_response, user, NULL, sock);
    }
    
    return 0;
}

int handle_main_menu(int sock, User *user, Vehicle *vehicle, char *message, char *server_response)
{
    display_menu(user);
    
    if (!fgets(message, DEFAULT_BUFLEN, stdin)) {
        printf("Greska tokom citanja ulaza\n");
        return 0;
    }
    message[strcspn(message, "\n")] = '\0';
    
    int choice = atoi(message);
    
    switch (choice) {
        case 1: // Logout
            snprintf(message, DEFAULT_BUFLEN, "# LOGOUT [%d] #", user->id);
            break;
        case 2: // Search
            {
                char search_id[20] = "null";
                char search_manufacturer[20] = "null";
                char search_model[20] = "null";
                char search_year[20] = "null";
                char input[256];
                char temp_message[200];
                
                printf("\n=== PRETRAGA VOZILA ===\n");
                printf("Ostavite prazno za parametre koje ne želite da koristite\n\n");
                
                printf("ID vozila: ");
                if (fgets(input, sizeof(input), stdin)) {
                    input[strcspn(input, "\n")] = '\0';
                    if (strlen(input) > 0) {
                        strncpy(search_id, input, sizeof(search_id) - 1);
                        search_id[sizeof(search_id) - 1] = '\0';
                    }
                }
                
                printf("Proizvođač: ");
                if (fgets(input, sizeof(input), stdin)) {
                    input[strcspn(input, "\n")] = '\0';
                    if (strlen(input) > 0) {
                        strncpy(search_manufacturer, input, sizeof(search_manufacturer) - 1);
                        search_manufacturer[sizeof(search_manufacturer) - 1] = '\0';
                    }
                }
                
                printf("Model: ");
                if (fgets(input, sizeof(input), stdin)) {
                    input[strcspn(input, "\n")] = '\0';
                    if (strlen(input) > 0) {
                        strncpy(search_model, input, sizeof(search_model) - 1);
                        search_model[sizeof(search_model) - 1] = '\0';
                    }
                }
                
                printf("Godina proizvodnje: ");
                if (fgets(input, sizeof(input), stdin)) {
                    input[strcspn(input, "\n")] = '\0';
                    if (strlen(input) > 0) {
                        strncpy(search_year, input, sizeof(search_year) - 1);
                        search_year[sizeof(search_year) - 1] = '\0';
                    }
                }
                
                // Format each parameter with brackets if not null
                strcpy(temp_message, "# SEARCH ");
                
                if (strcmp(search_id, "null") == 0) {
                    strcat(temp_message, "null ");
                } else {
                    strcat(temp_message, "[");
                    strcat(temp_message, search_id);
                    strcat(temp_message, "] ");
                }
                
                if (strcmp(search_manufacturer, "null") == 0) {
                    strcat(temp_message, "null ");
                } else {
                    strcat(temp_message, "[");
                    strcat(temp_message, search_manufacturer);
                    strcat(temp_message, "] ");
                }
                
                if (strcmp(search_model, "null") == 0) {
                    strcat(temp_message, "null ");
                } else {
                    strcat(temp_message, "[");
                    strcat(temp_message, search_model);
                    strcat(temp_message, "] ");
                }
                
                if (strcmp(search_year, "null") == 0) {
                    strcat(temp_message, "null");
                } else {
                    strcat(temp_message, "[");
                    strcat(temp_message, search_year);
                    strcat(temp_message, "]");
                }
                
                strcat(temp_message, " #");
                strcpy(message, temp_message);
                
                printf("DEBUG: Formatirana poruka za pretragu: %s\n", message);
                printf("\nPretragа je pokrenuta...\n");
            }
            break;
        case 3: // Reserve car
            {
                char car_id_input[20];
                printf("\n=== REZERVACIJA VOZILA ===\n");
                printf("Unesite ID vozila koje želite da rezervišete: ");
                
                if (fgets(car_id_input, sizeof(car_id_input), stdin)) {
                    car_id_input[strcspn(car_id_input, "\n")] = '\0';
                    int car_id = atoi(car_id_input);
                    vehicle->id = car_id;
                    if (car_id > 0) {
                        snprintf(message, DEFAULT_BUFLEN, "# RESERVE [%d] [%d] #", user->id, car_id);
                        printf("\nZahtev za rezervaciju je poslat...\n");
                    } else {
                        printf("Neispravno unet ID vozila.\n");
                        return 0;
                    }
                } else {
                    printf("Greška pri čitanju ID-a vozila.\n");
                    return 0;
                }
            }
            break;
        case 4: // Reserve
            snprintf(message, DEFAULT_BUFLEN, "Reserve");
            break;
        case 6: // Exit
            printf("Zatvaranje aplikacije...\n");
            return -1;
        default:
            printf("Nepoznata opcija. Molimo vas da unesete broj između 1 i 6.\n");
            return 0;
    }
    
    #ifdef DEBUG
        printf("Šalje se: %s\n", message);
    #endif
    
    // Send message to server
    if (send(sock, message, strlen(message), 0) < 0) {
        puts("Send failed");
        return 0;
    }
    printf("\nKomanda je uspešno poslata serveru.\n\n");
    
    // Receive response from server
    int read_size = recv(sock, server_response, DEFAULT_BUFLEN - 1, 0);
    if (read_size > 0) {
        server_response[read_size] = '\0';
        return process_server_response(server_response, user, NULL, sock);
    }
    
    return 1;
}

int process_server_response(char *server_response, User *user, int *authenticated, int sock)
{
    #ifdef DEBUG
        printf("Primljena poruka od servera:\n%s\n\n", server_response);
    #endif
    
    // Handle login errors
    if (strcmp(server_response, "# LOGIN ERROR 101 #") == 0) {
        printf("GRESKA: Uneli ste pogresnu lozinku!\n\n");
        return 0;
    }
    
    if (strcmp(server_response, "# LOGIN ERROR 102 #") == 0) {
        printf("GRESKA: Uneli ste pogresno korisnicko ime!\n\n");
        return 0;
    }
    
    // Handle registration errors
    if (strcmp(server_response, "# REGISTRATION 201 #") == 0) {
        printf("GRESKA: Korisnik vec postoji!\n\n");
        memset(user, 0, sizeof(User));
        return 0;
    }
    
    if (strcmp(server_response, "# REGISTRATION 202 #") == 0) {
        printf("GRESKA: Korisnicko ime je zauzeto!\n\n");
        memset(user, 0, sizeof(User));
        return 0;
    }

    // Handle logout response
    if (strcmp(server_response, "# LOGOUT SUCCES #") == 0) {
        printf("Uspešno ste se odjavili!\n\n");
        return 0; // Return to authentication menu
    }
    // Handle login success
    if (strncmp(server_response, "# LOGIN SUCCES", 14) == 0) {
        printf("Uspesno ste se prijavili!\n\n");
        parse_login_success(server_response, user);
        return 1;
    }
    
    // Handle registration success
    if (strncmp(server_response, "# REGISTRATION SUCCES", 21) == 0) {
        printf("Uspesno ste se registrovali!\n\n");
        parse_registration_success(server_response, user);
        return 1;
    }
     // Handle search responses
    if (strcmp(server_response, "# SEARCH SUCCES #") == 0) {
        printf("Pretraga je uspešna! Čekanje podataka o vozilima...\n\n");
        
        // Immediately wait for TABELA response
        int read_size = recv(sock, server_response, DEFAULT_BUFLEN - 1, 0);
        if (read_size > 0) {
            server_response[read_size] = '\0';
            
            #ifdef DEBUG
                printf("Primljena TABELA poruka:\n%s\n\n", server_response);
            #endif
            
            // Handle vehicle data response
            if (strncmp(server_response, "# TABELA", 8) == 0) {
                Vehicle vehicles[100]; // Maximum 100 vehicles
                int vehicle_count = parse_vehicle_data(server_response, vehicles, 100);
                
                if (vehicle_count > 0) {
                    display_vehicles_table(vehicles, vehicle_count);
                    return handle_vehicle_display_navigation();
                } else {
                    printf("Nema vozila za prikaz.\n\n");
                }
            } else {
                printf("Neočekivani odgovor nakon pretrage:\n%s\n\n", server_response);
            }
        }
        return 1; // Stay in main menu
    }
    
    if (strcmp(server_response, "# SEARCH ERROR #") == 0) {
        printf("GREŠKA: Pretraga nije uspešna!\n\n");
        return 1; // Stay in main menu
    }
    // Handle reserve responses
    if (strcmp(server_response, "# RESERVE SUCCES #") == 0) {
        printf("USPEH: Vozilo je uspešno rezervisano!\n\n");
        return 1; // Stay in main menu
    }
    
    if (strcmp(server_response, "# RESERVE ERROR 301 #") == 0) {
        printf("GREŠKA: Vozilo ne postoji!\n\n");
        return 1; // Stay in main menu
    }
    
    if (strcmp(server_response, "# RESERVE ERROR 302 #") == 0) {
        printf("GREŠKA: Vozilo je već zauzeto!\n\n");
        return 1; // Stay in main menu
    }
    
    if (strcmp(server_response, "# RESERVE ERROR 303 #") == 0) {
        printf("GREŠKA: Već ste rezervisali vozilo!\n\n");
        return 1; // Stay in main menu
    }
    
    // Handle other server responses
    printf("Odgovor servera:\n%s\n\n", server_response);
    return 1; // Stay in current menu
}

void cleanup_user_data(User *user)
{
    memset(user->first_name, 0, sizeof(user->first_name));
    memset(user->last_name, 0, sizeof(user->last_name));
    user->id = 0;
}

