#ifndef VOZILO_H
#define VOZILO_H

typedef struct {
    int id;
    char manufacturer[100];
    char carname[100];
    int year;
    char color[50];
    int status;  // 1 = available, 0 = reserved
    int reserved_by_user_id;  // -1 if not reserved, otherwise user ID
} Vozilo;

// Function declarations
void init_vozilo(Vozilo* vozilo, int id, const char* manufacturer, const char* carname, 
                 int year, const char* color);

#endif