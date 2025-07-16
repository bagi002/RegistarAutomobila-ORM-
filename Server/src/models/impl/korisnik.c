#include "../korisnik.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

void init_korisnik(Korisnik* korisnik, int id, const char* username, const char* password, const char* firstname, const char* lastname) {
    korisnik->id = id;
    strncpy(korisnik->username, username, 49);
    korisnik->username[49] = '\0';
    strncpy(korisnik->password, password, 255); // Jednostavno čuvanje šifre (demo)
    korisnik->password[255] = '\0';
    strncpy(korisnik->firstname, firstname, 49);
    korisnik->firstname[49] = '\0';
    strncpy(korisnik->lastname, lastname, 49);
    korisnik->lastname[49] = '\0';

    korisnik->status = 1; // aktivan
    korisnik->reserved_count = 0;
    
    // Inicijalizacija niza rezervisanih vozila
    for (int i = 0; i < 10; i++) {
        korisnik->reserved_vehicles[i] = -1;
    }
}

// Function to authenticate user
// Returns: 0 - success (both username and password match)
//          1 - username exists but password is wrong  
//          2 - username doesn't exist
int authenticate_korisnik(const char* username, const char* password, Korisnik* korisnik) {
    // Check if username matches
    if (strcmp(korisnik->username, username) == 0) {
        // Username found, now check password
        if (strcmp(korisnik->password, password) == 0) {
            // Both username and password match
            return 0;
        } else {
            // Username exists but password is wrong
            return 1;
        }
    }
    // Username not found
    return 2;
}

// Function to find user by username and password
// Returns: 0 - success (user found and authenticated)
//          101 - user found but wrong password
//          102 - user not found in database
// If successful, found_user will point to the authenticated user, otherwise NULL
int find_korisnik(const char* username, const char* password, Korisnik* korisnici_array, int korisnici_count, Korisnik** found_user) {
    *found_user = NULL;
    
    for (int i = 0; i < korisnici_count; i++) {
        int auth_result = authenticate_korisnik(username, password, &korisnici_array[i]);
        
        if (auth_result == 0) {
            // User found and authenticated successfully
            *found_user = &korisnici_array[i];
            return 0;
        } else if (auth_result == 1) {
            // User found but wrong password
            return 101;
        }
        // Continue searching if username doesn't match (auth_result == 2)
    }
    
    // User not found in database
    return 102;
}

// Function to check if username already exists
// Returns: 1 - username exists, 0 - username doesn't exist
int username_exists(const char* username, Korisnik* korisnici_array, int korisnici_count) {
    for (int i = 0; i < korisnici_count; i++) {
        if (strcmp(korisnici_array[i].username, username) == 0) {
            return 1; // Username exists
        }
    }
    return 0; // Username doesn't exist
}

// Function to register a new user
// Returns: 0 - success (user registered)
//          201 - user already exists
//          202 - username already taken
int register_korisnik(const char* firstname, const char* lastname, const char* username, 
                     const char* password, Korisnik* korisnici_array, int* korisnici_count, 
                     int max_count, int* new_user_id) {
    
    // Check if we have space for new user
    if (*korisnici_count >= max_count) {
        return 201; // Database full
    }
    
    // Check if username already exists
    if (username_exists(username, korisnici_array, *korisnici_count)) {
        return 202; // Username already taken
    }
    
    // Generate new user ID (highest existing ID + 1)
    int new_id = 1;
    for (int i = 0; i < *korisnici_count; i++) {
        if (korisnici_array[i].id >= new_id) {
            new_id = korisnici_array[i].id + 1;
        }
    }
    
    // Initialize the new user
    init_korisnik(&korisnici_array[*korisnici_count], new_id, username, password, firstname, lastname);
    
    // Set the new user ID for return
    *new_user_id = new_id;
    
    // Increment user count
    (*korisnici_count)++;
    
    return 0; // Success
}



