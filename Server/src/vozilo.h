#ifndef VOZILO_H
#define VOZILO_H

typedef struct {
    int id;
    char manufacturer[100];
    char carname[100];
    int year;
    char color[50];
    char license_plates[20];
    int status; // 0 = dostupno, 1 = rezervisano
    int reserved_by_user_id; // ID korisnika koji je rezervisao (-1 ako nije rezervisano)
    char reservation_date[20];
} Vozilo;

// Osnovni inicijalizator
void init_vozilo(Vozilo* vozilo, int id, const char* manufacturer, const char* carname, int year, const char* color, const char* license_plates);

#endif // VOZILO_H