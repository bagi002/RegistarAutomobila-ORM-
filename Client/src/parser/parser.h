#ifndef PARSER_H
#define PARSER_H

#include "../menu/menu.h"

// Function prototypes for parsing server responses
void parse_login_success(char *message, User *user);
void parse_registration_success(char *message, User *user);
int parse_vehicle_data(char *response, Vehicle *vehicles, int max_vehicles);

#endif // PARSER_H
