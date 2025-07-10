#include "../korisnik.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

void init_korisnik(Korisnik* korisnik, int id, const char* username, const char* password, const char* firstname, const char* lastname, const char* email) {
    korisnik->id = id;
    strncpy(korisnik->username, username, 49);
    korisnik->username[49] = '\0';
    strncpy(korisnik->password, password, 255); // Jednostavno čuvanje šifre (demo)
    korisnik->password[255] = '\0';
    strncpy(korisnik->firstname, firstname, 49);
    korisnik->firstname[49] = '\0';
    strncpy(korisnik->lastname, lastname, 49);
    korisnik->lastname[49] = '\0';
    strncpy(korisnik->email, email, 99);
    korisnik->email[99] = '\0';

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



