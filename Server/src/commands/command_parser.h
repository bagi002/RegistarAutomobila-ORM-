#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

// Command type enumeration
typedef enum {
    CMD_UNKNOWN = 0,
    CMD_LOGIN,
    CMD_REGISTRATION,
    CMD_LOGOUT,
    CMD_SEARCH,
    CMD_CHECKSTATUS,
    CMD_RESERVE
} command_t;

// Function declarations
int process_command(char *client_message, char komanda[1000][256], int thread_index);
command_t get_command_type(const char* cmd);
char* remove_brackets(const char* input);
int validate_command_args(command_t cmd_type, int arg_count, char komanda[1000][256]);
char* execute_command(command_t cmd_type, int arg_count, char komanda[1000][256], int thread_index);

#endif