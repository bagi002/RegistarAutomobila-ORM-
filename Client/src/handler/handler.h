#ifndef HANDLER_H
#define HANDLER_H

#include "../menu/menu.h"

// Function prototypes
int handle_authentication_menu(int sock, User *user, char *message, char *server_response);
int handle_main_menu(int sock, User *user, Vehicle *vehicle, char *message, char *server_response);
int process_server_response(char *server_response, User *user, int *authenticated, int sock);
void cleanup_user_data(User *user);

#endif // HANDLER_H
