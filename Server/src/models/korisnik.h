#ifndef KORISNIK_H
#define KORISNIK_H

typedef struct {
    int id;
    char username[50];
    char password[256];
    char firstname[50];
    char lastname[50];
    char email[100];
    int status;  // 1 = active, 0 = inactive
    int reserved_vehicles[10];  // Array of reserved vehicle IDs
    int reserved_count;  // Number of currently reserved vehicles
} Korisnik;

// Function declarations
void init_korisnik(Korisnik* korisnik, int id, const char* username, const char* password, 
                   const char* firstname, const char* lastname, const char* email);

// Function to authenticate user
// Returns: 0 - success (both username and password match)
//          1 - username exists but password is wrong  
//          2 - username doesn't exist
int authenticate_korisnik(const char* username, const char* password, Korisnik* korisnik);

// Function to find user by username and password
// Returns: 0 - success (user found and authenticated)
//          101 - user found but wrong password
//          102 - user not found in database
// If successful, found_user will point to the authenticated user, otherwise NULL
int find_korisnik(const char* username, const char* password, Korisnik* korisnici_array, int korisnici_count, Korisnik** found_user);

#endif