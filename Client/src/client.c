#include "menu.h"


// Uncomment the line below to enable debug output
#define DEBUG

// Function prototypes


int main(int argc, char *argv[])
{
    int sock;

    User user;
    Vehicle vehicle;
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
            result = handle_main_menu(sock, &user, &vehicle, message, server_response);
            if (result == -1) break; // Exit application
            if (result == 0) {
                authenticated = 0;
                cleanup_user_data(&user);
                continue;
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