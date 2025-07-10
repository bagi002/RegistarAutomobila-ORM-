#ifndef VOZILO_H
#define VOZILO_H

typedef struct {
    int id;
    char manufacturer[100];
    char carname[100];
    int year;
    char color[50];
    char tablice[20];  // License plates
    int status;  // 1 = available, 0 = reserved
    int reserved_by_user_id;  // -1 if not reserved, otherwise user ID
} Vozilo;

// Function declarations
void init_vozilo(Vozilo* vozilo, int id, const char* manufacturer, const char* carname, 
                 int year, const char* color, const char* tablice);

// Search function
int search_vozila(Vozilo* vozila_array, int vozila_count, 
                  const char* id_filter, const char* manufacturer_filter, 
                  const char* carname_filter, const char* year_filter,
                  Vozilo* results, int max_results);

// Function to send table data
void send_tabela(int sock, Vozilo* results, int result_count);

#endif