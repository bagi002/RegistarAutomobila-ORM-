#ifndef MENU_H
#define MENU_H

typedef struct {
    char first_name[20];
    char last_name[20];
    char name[20];
    char password[20];
} User;

void display_menu(const User* user);
void display_auth_menu();
void display_login_form();
void display_registration_form(User* user);

#endif // MENU_H
