#include "parser.h"

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
