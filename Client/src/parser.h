#ifndef PARSER_H
#define PARSER_H

#include "menu.h"

// Function prototypes for parsing server responses
void parse_login_success(char *message, User *user);
void parse_registration_success(char *message, User *user);

#endif // PARSER_H
