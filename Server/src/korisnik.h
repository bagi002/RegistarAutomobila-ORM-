#ifndef KORISNIK_H
#define KORISNIK_H

typedef struct {
    int id;
    char username[50];
    char password_hash[256]; // Hash šifre za bezbednost
    char firstname[50];
    char lastname[50];
    char email[100];
    char registration_date[20];
    int status; // 0 = neaktivan, 1 = aktivan
    int reserved_vehicles[10]; // Niz ID-jeva rezervisanih vozila (maksimalno 10)
    int reserved_count; // Broj rezervisanih vozila
} Korisnik;

// Osnovni inicijalizator
void init_korisnik(Korisnik* korisnik, int id, const char* username, const char* password, const char* firstname, const char* lastname, const char* email);

#endif // KORISNIK_H