#include "data_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to load users from korisnici.txt file
int load_korisnici_from_file(const char* filepath, Korisnik* korisnici_array, int max_count) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Warning: Could not open %s. Starting with empty user array.\n", filepath);
        return 0;
    }
    
    int count = 0;
    char line[512];
    
    while (fgets(line, sizeof(line), file) && count < max_count) {
        // Parse line format: id,username,password,firstname,lastname,status
        char* token = strtok(line, ",");
        if (token == NULL) continue;
        
        korisnici_array[count].id = atoi(token);
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(korisnici_array[count].username, token, 49);
        korisnici_array[count].username[49] = '\0';
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(korisnici_array[count].password, token, 255);
        korisnici_array[count].password[255] = '\0';
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(korisnici_array[count].firstname, token, 49);
        korisnici_array[count].firstname[49] = '\0';
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(korisnici_array[count].lastname, token, 49);
        korisnici_array[count].lastname[49] = '\0';
        
        token = strtok(NULL, ",\n");
        if (token == NULL) continue;
        korisnici_array[count].status = atoi(token);
        
        // Initialize reserved vehicles array
        korisnici_array[count].reserved_count = 0;
        for (int i = 0; i < 10; i++) {
            korisnici_array[count].reserved_vehicles[i] = -1;
        }
        
        count++;
    }
    
    fclose(file);
    printf("Loaded %d users from %s\n", count, filepath);
    return count;
}

// Function to load vehicles from vozila.txt file
int load_vozila_from_file(const char* filepath, Vozilo* vozila_array, int max_count) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Warning: Could not open %s. Starting with empty vehicle array.\n", filepath);
        return 0;
    }
    
    int count = 0;
    char line[512];
    
    while (fgets(line, sizeof(line), file) && count < max_count) {
        // Parse line format: id,manufacturer,carname,year,color,status,reserved_by_user_id
        char* token = strtok(line, ",");
        if (token == NULL) continue;
        
        vozila_array[count].id = atoi(token);
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(vozila_array[count].manufacturer, token, 99);
        vozila_array[count].manufacturer[99] = '\0';
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(vozila_array[count].carname, token, 99);
        vozila_array[count].carname[99] = '\0';
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        vozila_array[count].year = atoi(token);
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(vozila_array[count].color, token, 49);
        vozila_array[count].color[49] = '\0';
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        vozila_array[count].status = atoi(token);
        
        token = strtok(NULL, ",\n");
        if (token != NULL) {
            vozila_array[count].reserved_by_user_id = atoi(token);
        } else {
            vozila_array[count].reserved_by_user_id = -1;
        }
        
        count++;
    }
    
    fclose(file);
    printf("Loaded %d vehicles from %s\n", count, filepath);
    return count;
}