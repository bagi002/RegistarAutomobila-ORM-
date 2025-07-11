#include <stdio.h>
#include <string.h>
#include "menu.h"

void display_menu(const User* user)
{
    puts("\n");
    puts("╔══════════════════════════════════════════════════════════════════════════════╗");
    puts("║                                                                              ║");
    puts("║                          REGISTAR AUTOMOBILA                                 ║");
    puts("║                         Sistem za upravljanje                                ║");
    puts("║                                                                              ║");
    puts("╠══════════════════════════════════════════════════════════════════════════════╣");
    printf("║                                                                              ║\n");
    printf("║  Dobrodošli, %s %s!                                           ║\n", user->first_name, user->last_name);
    puts("║                                                                              ║");
    puts("║  ┌─────────────────────────────────────────────────────────────────────┐     ║");
    puts("║  │                        DOSTUPNE KOMANDE                             │     ║");
    puts("║  ├─────────────────────────────────────────────────────────────────────┤     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [1] ➤ Odjava korisnika                                             │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [2] ➤ Pretraga dostupnih automobila u elektronskom registru        │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [3] ➤ Pretraga svih automobila u elektronskom registru             │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [4] ➤ Provera rezervisanih automobila prijavljenog korisnika       │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [5] ➤ Rezervacija automobila po ID-u automobila                    │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [6] ➤ Izlaz iz aplikacije                                          │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  └─────────────────────────────────────────────────────────────────────┘     ║");
    puts("║                                                                              ║");
    puts("╚══════════════════════════════════════════════════════════════════════════════╝");
    puts("");
    printf("Vaš izbor: ");
}

void display_auth_menu()
{
    puts("\n");
    puts("╔══════════════════════════════════════════════════════════════════════════════╗");
    puts("║                                                                              ║");
    puts("║                          REGISTAR AUTOMOBILA                                 ║");
    puts("║                         Sistem za upravljanje                                ║");
    puts("║                                                                              ║");
    puts("╠══════════════════════════════════════════════════════════════════════════════╣");
    puts("║                                                                              ║");
    puts("║  Molimo vas da se prijavite ili registrujete:                                ║");
    puts("║                                                                              ║");
    puts("║  ┌─────────────────────────────────────────────────────────────────────┐     ║");
    puts("║  │                        DOSTUPNE OPCIJE                              │     ║");
    puts("║  ├─────────────────────────────────────────────────────────────────────┤     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [1] ➤ Prijava postojećeg korisnika                                 │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [2] ➤ Registracija novog korisnika                                 │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  │  [3] ➤ Izlazak iz aplikacije                                        │     ║");
    puts("║  │                                                                     │     ║");
    puts("║  └─────────────────────────────────────────────────────────────────────┘     ║");
    puts("║                                                                              ║");
    puts("╚══════════════════════════════════════════════════════════════════════════════╝");
    puts("");
    printf("Vaš izbor: ");
}

void display_login_form()
{
    puts("\n");
    puts("╔══════════════════════════════════════════════════════════════════════════════╗");
    puts("║                                                                              ║");
    puts("║                            PRIJAVA KORISNIKA                                 ║");
    puts("║                                                                              ║");
    puts("╠══════════════════════════════════════════════════════════════════════════════╣");
    puts("║                                                                              ║");
    puts("║  Molimo vas da unesete vaše podatke za prijavu:                             ║");
    puts("║                                                                              ║");
    puts("╚══════════════════════════════════════════════════════════════════════════════╝");
    puts("");
}

void display_registration_form(User* user)
{
    puts("\n");
    puts("╔══════════════════════════════════════════════════════════════════════════════╗");
    puts("║                                                                              ║");
    puts("║                          REGISTRACIJA KORISNIKA                              ║");
    puts("║                                                                              ║");
    puts("╠══════════════════════════════════════════════════════════════════════════════╣");
    puts("║                                                                              ║");
    puts("║  Molimo vas da unesete vaše podatke za registraciju:                         ║");
    puts("║                                                                              ║");
    puts("╚══════════════════════════════════════════════════════════════════════════════╝");
    puts("");
    
    printf("Ime: ");
    if(fgets(user->first_name, sizeof(user->first_name), stdin))
        user->first_name[strcspn(user->first_name, "\n")] = '\0';
    
    printf("Prezime: ");
    if(fgets(user->last_name, sizeof(user->last_name), stdin))
        user->last_name[strcspn(user->last_name, "\n")] = '\0';
    
    printf("Korisničko ime: ");
    if(fgets(user->name, sizeof(user->name), stdin))
        user->name[strcspn(user->name, "\n")] = '\0';
    
    printf("Lozinka: ");
    if(fgets(user->password, sizeof(user->password), stdin))
        user->password[strcspn(user->password, "\n")] = '\0';
    
    puts("\nPodaci su uspešno uneti!");
}
