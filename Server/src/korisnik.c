#include "korisnik.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

void init_korisnik(Korisnik* korisnik, int id, const char* username, const char* password, const char* firstname, const char* lastname, const char* email) {
    korisnik->id = id;
    strncpy(korisnik->username, username, 49);
    korisnik->username[49] = '\0';
    strncpy(korisnik->password_hash, password, 255); // Jednostavno čuvanje šifre (demo)
    korisnik->password_hash[255] = '\0';
    strncpy(korisnik->firstname, firstname, 49);
    korisnik->firstname[49] = '\0';
    strncpy(korisnik->lastname, lastname, 49);
    korisnik->lastname[49] = '\0';
    strncpy(korisnik->email, email, 99);
    korisnik->email[99] = '\0';
    
    // Postavljanje datuma registracije
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    strftime(korisnik->registration_date, 20, "%Y-%m-%d", timeinfo);
    
    korisnik->status = 1; // aktivan
    korisnik->reserved_count = 0;
    
    // Inicijalizacija niza rezervisanih vozila
    for (int i = 0; i < 10; i++) {
        korisnik->reserved_vehicles[i] = -1;
    }
}