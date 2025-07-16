#include "../vozilo.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "../../network/network_utils.h"

void init_vozilo(Vozilo* vozilo, int id, const char* manufacturer, const char* carname, 
                 int year, const char* color, const char* tablice) {
    vozilo->id = id;
    strncpy(vozilo->manufacturer, manufacturer, 99);
    vozilo->manufacturer[99] = '\0';
    strncpy(vozilo->carname, carname, 99);
    vozilo->carname[99] = '\0';
    vozilo->year = year;
    strncpy(vozilo->color, color, 49);
    vozilo->color[49] = '\0';
    strncpy(vozilo->tablice, tablice, 19);
    vozilo->tablice[19] = '\0';
    vozilo->status = 1; // Available by default
    vozilo->reserved_by_user_id = -1; // Not reserved
}

// Search function that filters vehicles based on provided criteria
int search_vozila(Vozilo* vozila_array, int vozila_count, 
                  const char* id_filter, const char* manufacturer_filter, 
                  const char* carname_filter, const char* year_filter,
                  Vozilo* results, int max_results) {
    int result_count = 0;
    
    for (int i = 0; i < vozila_count && result_count < max_results; i++) {
        int matches = 1;
        
        // Check ID filter
        if (id_filter != NULL && strcmp(id_filter, "null") != 0) {
            int filter_id = atoi(id_filter);
            if (vozila_array[i].id != filter_id) {
                matches = 0;
            }
        }
        
        // Check manufacturer filter
        if (matches && manufacturer_filter != NULL && strcmp(manufacturer_filter, "null") != 0) {
            if (strstr(vozila_array[i].manufacturer, manufacturer_filter) == NULL) {
                matches = 0;
            }
        }
        
        // Check carname filter
        if (matches && carname_filter != NULL && strcmp(carname_filter, "null") != 0) {
            if (strstr(vozila_array[i].carname, carname_filter) == NULL) {
                matches = 0;
            }
        }
        
        // Check year filter
        if (matches && year_filter != NULL && strcmp(year_filter, "null") != 0) {
            int filter_year = atoi(year_filter);
            if (vozila_array[i].year != filter_year) {
                matches = 0;
            }
        }
        
        if (matches) {
            results[result_count] = vozila_array[i];
            result_count++;
        }
    }
    
    return result_count;
}

// Function to send table data to client
void send_tabela(int sock, Vozilo* results, int result_count) {
    char message[8192];  // Increased buffer size
    char temp[512];
    
    // Start building the TABELA message
    snprintf(message, sizeof(message), "# TABELA [%d] ", result_count);
    
    for (int i = 0; i < result_count; i++) {
        snprintf(temp, sizeof(temp), "{[%d] [%s] [%s] [%d] [%s] [%s] [%s]} ",
                results[i].id,
                results[i].manufacturer,
                results[i].carname,
                results[i].year,
                results[i].color,
                results[i].tablice,
                results[i].status == 1 ? "Dostupno" : "Zauzeto");
        
        // Check if adding this vehicle would exceed message buffer
        if (strlen(message) + strlen(temp) + 2 < sizeof(message)) {
            strcat(message, temp);
        } else {
            // If message is too long, break and send what we have
            break;
        }
    }
    
    strcat(message, "#");
    send_message(sock, message);
}