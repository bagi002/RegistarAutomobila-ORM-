#ifndef KORISNIK_H
#define KORISNIK_H

typedef struct {
    int id;
    char username[50];
    char password[256];
    char firstname[50];
    char lastname[50];
    int status;  // 1 = active, 0 = inactive
    int reserved_vehicles[10];  // Array of reserved vehicle IDs
    int reserved_count;  // Number of currently reserved vehicles
} Korisnik;

// Function declarations
void init_korisnik(Korisnik* korisnik, int id, const char* username, const char* password, 
                   const char* firstname, const char* lastname);

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

// Function to check if username already exists
// Returns: 1 - username exists, 0 - username doesn't exist
int username_exists(const char* username, Korisnik* korisnici_array, int korisnici_count);

// Function to register a new user
// Returns: 0 - success (user registered)
//          201 - user already exists
//          202 - username already taken
int register_korisnik(const char* firstname, const char* lastname, const char* username, 
                     const char* password, Korisnik* korisnici_array, int* korisnici_count, 
                     int max_count, int* new_user_id);

#endif