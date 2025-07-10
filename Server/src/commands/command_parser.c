#include "command_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_BUFLEN 1024

// Function to remove brackets from string (removes first and last character if they are brackets)
char* remove_brackets(const char* input) {
    int len = strlen(input);
    char* result = malloc(256);
    
    if(result == NULL) {
        return NULL;
    }
    
    // Check if string has brackets at start and end
    if(len >= 2 && input[0] == '[' && input[len-1] == ']') {
        // Copy string without first and last character
        strncpy(result, input + 1, len - 2);
        result[len - 2] = '\0';
    } else {
        // No brackets, return original string
        strcpy(result, input);
    }
    
    return result;
}

// Helper function to convert command string to enum
command_t get_command_type(const char* cmd) {
    if(strcmp(cmd, "LOGIN") == 0) return CMD_LOGIN;
    if(strcmp(cmd, "REGISTRATION") == 0) return CMD_REGISTRATION;
    if(strcmp(cmd, "LOGOUT") == 0) return CMD_LOGOUT;
    if(strcmp(cmd, "SEARCH") == 0) return CMD_SEARCH;
    if(strcmp(cmd, "CHECKSTATUS") == 0) return CMD_CHECKSTATUS;
    if(strcmp(cmd, "RESERVE") == 0) return CMD_RESERVE;
    return CMD_UNKNOWN;
}

// Funkcija preuzima komandu u formatu: # word word ... word #
// i vraca niz riječi u komanda[1000][256]
int process_command(char *client_message, char komanda[1000][256], int thread_index) {
    int broj_rijeci = 0;
    
    int len = strlen(client_message);
    if(len >= 3 && client_message[0] == '#' && client_message[len-1] == '#') {
        printf("Thread %d: Processing command format\n", thread_index);
        
        char temp_message[DEFAULT_BUFLEN];
        strcpy(temp_message, client_message);
        
        temp_message[len-1] = '\0';
        char *content = temp_message + 1;
        
        char *token = strtok(content, " \t\n");
        while(token != NULL && broj_rijeci < 1000) {
            strncpy(komanda[broj_rijeci], token, 255);
            komanda[broj_rijeci][255] = '\0';
            broj_rijeci++;
            token = strtok(NULL, " \t\n");
        }
        
        printf("Thread %d: Parsed %d words: ", thread_index, broj_rijeci);
        for(int i = 0; i < broj_rijeci; i++) {
            printf("'%s' ", komanda[i]);
        }
        printf("\n");
        
        return broj_rijeci;
        
    } else {
        printf("Thread %d: Invalid command format. Expected: # word word ... word #\n", thread_index);
        return -1;
    }
}