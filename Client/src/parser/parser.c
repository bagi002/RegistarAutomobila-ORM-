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

// Function to parse vehicle data from server response
int parse_vehicle_data(char *response, Vehicle *vehicles, int max_vehicles) {
    int vehicle_count = 0;
    char *ptr = response;
    char *start, *end;
    
    // Find the start of TABELA
    if (strncmp(ptr, "# TABELA", 8) != 0) {
        return 0;
    }
    
    // Skip to the first vehicle data
    ptr = strchr(ptr, '{');
    
    while (ptr != NULL && vehicle_count < max_vehicles) {
        ptr++; // Skip '{'
        
        // Parse ID
        if (*ptr == '[') {
            ptr++;
            start = ptr;
            end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                vehicles[vehicle_count].id = atoi(start);
                ptr = end + 1;
            }
        }
        
        // Skip space and parse manufacturer
        while (*ptr == ' ') ptr++;
        if (*ptr == '[') {
            ptr++;
            start = ptr;
            end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                strncpy(vehicles[vehicle_count].manufacturer, start, sizeof(vehicles[vehicle_count].manufacturer) - 1);
                vehicles[vehicle_count].manufacturer[sizeof(vehicles[vehicle_count].manufacturer) - 1] = '\0';
                ptr = end + 1;
            }
        }
        
        // Skip space and parse model
        while (*ptr == ' ') ptr++;
        if (*ptr == '[') {
            ptr++;
            start = ptr;
            end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                strncpy(vehicles[vehicle_count].model, start, sizeof(vehicles[vehicle_count].model) - 1);
                vehicles[vehicle_count].model[sizeof(vehicles[vehicle_count].model) - 1] = '\0';
                ptr = end + 1;
            }
        }
        
        // Skip space and parse year
        while (*ptr == ' ') ptr++;
        if (*ptr == '[') {
            ptr++;
            start = ptr;
            end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                vehicles[vehicle_count].year = atoi(start);
                ptr = end + 1;
            }
        }
        
        // Skip space and parse color
        while (*ptr == ' ') ptr++;
        if (*ptr == '[') {
            ptr++;
            start = ptr;
            end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                strncpy(vehicles[vehicle_count].color, start, sizeof(vehicles[vehicle_count].color) - 1);
                vehicles[vehicle_count].color[sizeof(vehicles[vehicle_count].color) - 1] = '\0';
                ptr = end + 1;
            }
        }
        
        // Skip space and parse license plate
        while (*ptr == ' ') ptr++;
        if (*ptr == '[') {
            ptr++;
            start = ptr;
            end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                strncpy(vehicles[vehicle_count].license_plate, start, sizeof(vehicles[vehicle_count].license_plate) - 1);
                vehicles[vehicle_count].license_plate[sizeof(vehicles[vehicle_count].license_plate) - 1] = '\0';
                ptr = end + 1;
            }
        }
        
        // Skip space and parse status
        while (*ptr == ' ') ptr++;
        if (*ptr == '[') {
            ptr++;
            start = ptr;
            end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                strncpy(vehicles[vehicle_count].status, start, sizeof(vehicles[vehicle_count].status) - 1);
                vehicles[vehicle_count].status[sizeof(vehicles[vehicle_count].status) - 1] = '\0';
                ptr = end + 1;
            }
        }
        
        vehicle_count++;
        
        // Find next vehicle or end
        ptr = strchr(ptr, '{');
    }
    
    return vehicle_count;
}