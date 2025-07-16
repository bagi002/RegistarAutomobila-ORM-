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

int load_vozila_from_file(const char* filename, Vozilo* vozila, int max_count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }
    
    int count = 0;
    char line[512];
    
    while (fgets(line, sizeof(line), file) && count < max_count) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;
        
        // Parse CSV line: id,manufacturer,carname,year,color,tablice,status,reserved_by_user_id
        char* id_str = strtok(line, ",");
        char* manufacturer = strtok(NULL, ",");
        char* carname = strtok(NULL, ",");
        char* year_str = strtok(NULL, ",");
        char* color = strtok(NULL, ",");
        char* tablice = strtok(NULL, ",");
        char* status_str = strtok(NULL, ",");
        char* reserved_by_str = strtok(NULL, ",");
        
        if (id_str && manufacturer && carname && year_str && color && tablice && status_str && reserved_by_str) {
            int id = atoi(id_str);
            int year = atoi(year_str);
            int status = atoi(status_str);
            int reserved_by = atoi(reserved_by_str);
            
            init_vozilo(&vozila[count], id, manufacturer, carname, year, color, tablice);
            vozila[count].status = status;
            vozila[count].reserved_by_user_id = reserved_by;
            count++;
        } else {
            printf("Warning: Invalid line format in %s: %s\n", filename, line);
        }
    }
    
    fclose(file);
    printf("Loaded %d vehicles from %s\n", count, filename);
    return count;
}