#include "menu.h"

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
        return process_server_response(server_response, user, NULL);
    }
    
    return 0;
}

int handle_main_menu(int sock, User *user, char *message, char *server_response)
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
            snprintf(message, DEFAULT_BUFLEN, "Search");
            break;
        case 3: // SearchAll
            snprintf(message, DEFAULT_BUFLEN, "SearchAll");
            break;
        case 4: // CheckStatus
            snprintf(message, DEFAULT_BUFLEN, "CheckStatus");
            break;
        case 5: // Reserve
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
        
        if (strcmp(server_response, "# LOGOUT SUCCES #") == 0) {
            printf("Uspešno ste se odjavili!\n\n");
            return 0; // Return to authentication menu
        }
        
        printf("Odgovor servera:\n%s\n\n", server_response);
    }
    
    return 1; // Stay in main menu
}

int process_server_response(char *server_response, User *user, int *authenticated)
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
    
    // Handle login success
    if (strncmp(server_response, "# LOGIN SUCCES", 14) == 0) {
        printf("Uspesno ste se prijavili!\n\n");
        parse_login_success(server_response, user);
        return 1;
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
    
    // Handle registration success
    if (strncmp(server_response, "# REGISTRATION SUCCES", 21) == 0) {
        printf("Uspesno ste se registrovali!\n\n");
        parse_registration_success(server_response, user);
        return 1;
    }
    
    return 0;
}

void parse_login_success(char *message, User *user)
{
    // Format: "# LOGIN SUCCES ID [ime] [prezime] #"
    char temp_message[DEFAULT_BUFLEN];
    strncpy(temp_message, message, DEFAULT_BUFLEN - 1);
    temp_message[DEFAULT_BUFLEN - 1] = '\0';
    
    // Parse ID between "SUCCES" and first "["
    char *id_start = strstr(temp_message, "SUCCES");
    if (id_start != NULL) {
        id_start += 7; // Skip "SUCCES "
        char *id_end = strchr(id_start, '[');
        if (id_end != NULL) {
            *id_end = '\0';
            // Remove spaces
            while (*id_start == ' ') id_start++;
            id_end--;
            while (id_end > id_start && *id_end == ' ') {
                *id_end = '\0';
                id_end--;
            }
            user->id = atoi(id_start);
            
            // Restore original string for further parsing
            strncpy(temp_message, message, DEFAULT_BUFLEN - 1);
            temp_message[DEFAULT_BUFLEN - 1] = '\0';
        }
    }
    
    // Parse first and last name
    char *start = strchr(temp_message, '[');
    if (start != NULL) {
        start++; // Skip '['
        char *end = strchr(start, ']');
        if (end != NULL) {
            *end = '\0';
            strncpy(user->first_name, start, sizeof(user->first_name) - 1);
            user->first_name[sizeof(user->first_name) - 1] = '\0';
            
            // Find second bracket
            start = strchr(end + 1, '[');
            if (start != NULL) {
                start++; // Skip '['
                end = strchr(start, ']');
                if (end != NULL) {
                    *end = '\0';
                    strncpy(user->last_name, start, sizeof(user->last_name) - 1);
                    user->last_name[sizeof(user->last_name) - 1] = '\0';
                }
            }
        }
    }
    
    #ifdef DEBUG
        printf("Korisnikov ID: %d\n", user->id);
        printf("Korisnikovo ime: %s\n", user->first_name);
        printf("Korisnikovo prezime: %s\n", user->last_name);
    #endif
}

void parse_registration_success(char *message, User *user)
{
    // Format: "# REGISTRATION SUCCES ID #"
    char temp_message[DEFAULT_BUFLEN];
    strncpy(temp_message, message, DEFAULT_BUFLEN - 1);
    temp_message[DEFAULT_BUFLEN - 1] = '\0';
    
    // Find ID between "SUCCES" and "#"
    char *start = strstr(temp_message, "SUCCES");
    if (start != NULL) {
        start += 7; // Skip "SUCCES "
        char *end = strchr(start, '#');
        if (end != NULL) {
            *end = '\0';
            // Remove spaces
            while (*start == ' ') start++;
            end--;
            while (end > start && *end == ' ') {
                *end = '\0';
                end--;
            }
            user->id = atoi(start);
            
            #ifdef DEBUG
                printf("Korisnikov ID: %d\n", user->id);
            #endif
        }
    }
}

void cleanup_user_data(User *user)
{
    memset(user->first_name, 0, sizeof(user->first_name));
    memset(user->last_name, 0, sizeof(user->last_name));
    user->id = 0;
}

