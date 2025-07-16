#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include "../models/korisnik.h"
#include "../models/vozilo.h"

// Function declarations
int load_korisnici_from_file(const char* filepath, Korisnik* korisnici_array, int max_count);
int load_vozila_from_file(const char* filepath, Vozilo* vozila_array, int max_count);

#endif