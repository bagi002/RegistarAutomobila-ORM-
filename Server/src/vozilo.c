#include "vozilo.h"
#include <string.h>

void init_vozilo(Vozilo* vozilo, int id, const char* manufacturer, const char* carname, int year, const char* color, const char* license_plates) {
    vozilo->id = id;
    strncpy(vozilo->manufacturer, manufacturer, 99);
    vozilo->manufacturer[99] = '\0';
    strncpy(vozilo->carname, carname, 99);
    vozilo->carname[99] = '\0';
    vozilo->year = year;
    strncpy(vozilo->color, color, 49);
    vozilo->color[49] = '\0';
    strncpy(vozilo->license_plates, license_plates, 19);
    vozilo->license_plates[19] = '\0';
    vozilo->status = 0; // dostupno
    vozilo->reserved_by_user_id = -1;
    strcpy(vozilo->reservation_date, "");
}