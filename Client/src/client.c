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


#include "menu.h"

// Uncomment the line below to enable debug output
#define DEBUG

// Function prototypes


int main(int argc, char *argv[])
{
    int sock;
    User user;
    int authenticated = 0;
    
    // Initialize user structure
    memset(&user, 0, sizeof(User));
    
    // Create socket connection
    sock = create_socket_connection();
    if (sock == -1) {
        return 1;
    }
    
    // Allocate memory for messages
    char *message = (char*)malloc(DEFAULT_BUFLEN * sizeof(char));
    char *server_response = (char*)malloc(DEFAULT_BUFLEN * sizeof(char));
    
    if (message == NULL || server_response == NULL) {
        printf("Memory allocation failed.\n");
        if (message) free(message);
        if (server_response) free(server_response);
        close(sock);
        return 1;
    }
    
    // Main application loop
    while (1) {
        int result;
        
        if (!authenticated) {
            result = handle_authentication_menu(sock, &user, message, server_response);
            if (result == -1) break; // Exit application
            if (result == 1) {
                authenticated = 1;
                continue;
            }
        } else {
            result = handle_main_menu(sock, &user, message, server_response);
            if (result == -1) break; // Exit application
            
            // Receive response from server after sending command
            int read_size = recv(sock, server_response, DEFAULT_BUFLEN - 1, 0);
            if (read_size > 0) {
                server_response[read_size] = '\0';
                int process_result = process_server_response(server_response, &user, &authenticated);
                
                if (process_result == 0) {
                    authenticated = 0;
                    cleanup_user_data(&user);
                    continue;
                }
            } else if (read_size == 0) {
                printf("Server je zatvorio konekciju.\n");
                break;
            } else {
                perror("recv failed");
                break;
            }
        }
    }
    
    // Cleanup
    printf("Hvala Vam sto ste posetili nasu stranicu :)\n");
    free(message);
    free(server_response);
    close(sock);
    
    return 0;
}