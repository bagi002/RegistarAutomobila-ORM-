#include "client_handler.h"
#include "../network/network_utils.h"
#include "../commands/command_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../models/korisnik.h"
#include "../models/vozilo.h"

#define DEFAULT_BUFLEN 1024

extern Korisnik korisnici[100];
extern Vozilo vozila[100];
extern int broj_korisnika;
extern int broj_vozila;

// Global variables definitions
int thread_status[10] = {0};
pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

// FUnkcija za obradu komunikacije sa klijentom
void *connection_handler(void *param)
{
    // Get thread index and socket from parameter
    int *params = (int*)param;
    int thread_index = params[0];
    int sock = params[1];
    
    // Detach the thread so it cleans up automatically
    pthread_detach(pthread_self());
    
    int read_size;
    char client_message[DEFAULT_BUFLEN];

    printf("Thread %d started for client\n", thread_index);

    // Main message receiving loop
    while((read_size = receive_message(sock, client_message, DEFAULT_BUFLEN)) > 0)
    {
        printf("Thread %d received: %s\n", thread_index, client_message);
        
        // Check if client wants to exit
        if(strcmp(client_message, "exit") == 0) {
            printf("Thread %d: Client requested exit\n", thread_index);
            // Send confirmation message before closing
            send_message(sock, "SERVER: Goodbye!");
            break;
        }
        
        // Parse command using the separate function
        char komanda[1000][256];  // Array to store parsed words
        int broj_rijeci = process_command(client_message, komanda, thread_index);
        
        if(broj_rijeci < 0) {
            // Invalid command format
            char error_response[DEFAULT_BUFLEN];
            strcpy(error_response, "SERVER: Invalid format. Use: # command word ... #");
            if(send_message(sock, error_response) < 0) {
                printf("Thread %d: Send failed\n", thread_index);
                break;
            }
        } else {
            // Process the parsed command using simple switch
            char response[DEFAULT_BUFLEN];
            int err = 0;
            
            if(broj_rijeci > 0) {
                command_t cmd_type = get_command_type(komanda[0]);
                
                switch(cmd_type) {
                    case CMD_LOGIN:
                        if (broj_rijeci < 3){
                            if(send_message(sock, "# LOGIN ERROR 103 #") < 0) {
                                printf("Thread %d: Send failed\n", thread_index);
                                break;
                            }
                        }else{
                            char *user_name = remove_brackets(komanda[1]);
                            char *user_password = remove_brackets(komanda[2]);

                            Korisnik* found_user;
                            int result = find_korisnik(user_name, user_password, korisnici, broj_korisnika, &found_user);
                            
                            if (result == 0) {
                                char success_msg[DEFAULT_BUFLEN];
                                snprintf(success_msg, sizeof(success_msg), "# LOGIN SUCCES %d [%s] [%s] #", 
                                    found_user->id, found_user->firstname, found_user->lastname);
                                if(send_message(sock, success_msg) < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            } else if (result == 101) {
                                if(send_message(sock, "# LOGIN ERROR 101 #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            } else if (result == 102) {
                                if(send_message(sock, "# LOGIN ERROR 102 #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            }
                            
                            free(user_name);
                            free(user_password);
                        }
                        continue;
                        
                    case CMD_REGISTRATION:
                        if (broj_rijeci < 5){
                            if(send_message(sock, "# REGISTRATION ERROR 203 #") < 0) {
                                printf("Thread %d: Send failed\n", thread_index);
                                break;
                            }
                        }else{
                            char *firstname = remove_brackets(komanda[1]);
                            char *lastname = remove_brackets(komanda[2]);
                            char *username = remove_brackets(komanda[3]);
                            char *password = remove_brackets(komanda[4]);

                            int new_user_id;
                            int result = register_korisnik(firstname, lastname, username, password, 
                                                         korisnici, &broj_korisnika, 100, &new_user_id);
                            
                            if (result == 0) {
                                char success_msg[DEFAULT_BUFLEN];
                                snprintf(success_msg, sizeof(success_msg), "# REGISTRATION SUCCES %d #", new_user_id);
                                if(send_message(sock, success_msg) < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            } else if (result == 201) {
                                if(send_message(sock, "# REGISTRATION ERROR 201 #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            } else if (result == 202) {
                                if(send_message(sock, "# REGISTRATION ERROR 202 #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            }
                            
                            free(firstname);
                            free(lastname);
                            free(username);
                            free(password);
                        }
                        continue;
                        
                    case CMD_LOGOUT:
                        if (broj_rijeci < 2){
                            if(send_message(sock, "# LOGOUT ERROR #") < 0) {
                                printf("Thread %d: Send failed\n", thread_index);
                                break;
                            }
                        }else{
                            char *user_id_str = remove_brackets(komanda[1]);
                            int user_id = atoi(user_id_str);
                            
                            // Check if user exists
                            int user_found = 0;
                            for (int i = 0; i < broj_korisnika; i++) {
                                if (korisnici[i].id == user_id) {
                                    user_found = 1;
                                    break;
                                }
                            }
                            
                            if (user_found) {
                                if(send_message(sock, "# LOGOUT SUCCES #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            } else {
                                if(send_message(sock, "# LOGOUT ERROR #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            }
                            
                            free(user_id_str);
                        }
                        continue;
                        
                    case CMD_SEARCH:
                        if (broj_rijeci < 5) {
                            if(send_message(sock, "# SEARCH ERROR #") < 0) {
                                printf("Thread %d: Send failed\n", thread_index);
                                break;
                            }
                        } else {
                            char *id_filter = remove_brackets(komanda[1]);
                            char *manufacturer_filter = remove_brackets(komanda[2]);
                            char *carname_filter = remove_brackets(komanda[3]);
                            char *year_filter = remove_brackets(komanda[4]);
                            
                            // Perform search
                            Vozilo results[100];
                            int result_count = search_vozila(vozila, broj_vozila, 
                                                           id_filter, manufacturer_filter, 
                                                           carname_filter, year_filter,
                                                           results, 100);
                            
                            // Send success response first
                            if(send_message(sock, "# SEARCH SUCCES #") < 0) {
                                printf("Thread %d: Send failed\n", thread_index);
                                break;
                            }
                            
                            // Then send the table data
                            send_tabela(sock, results, result_count);
                            
                            free(id_filter);
                            free(manufacturer_filter);
                            free(carname_filter);
                            free(year_filter);
                        }
                        continue;
                        
                    case CMD_CHECKSTATUS:
                        if (broj_rijeci < 2){
                            if(send_message(sock, "# CHECKSTATUS ERROR #") < 0) {
                                printf("Thread %d: Send failed\n", thread_index);
                                break;
                            }
                        }else{
                            char *user_id_str = remove_brackets(komanda[1]);
                            int user_id = atoi(user_id_str);
                            
                            // Check if user exists
                            int user_found = 0;
                            for (int i = 0; i < broj_korisnika; i++) {
                                if (korisnici[i].id == user_id) {
                                    user_found = 1;
                                    break;
                                }
                            }
                            
                            if (user_found) {
                                // Find all vehicles reserved by this user
                                Vozilo reserved_vehicles[100];
                                int reserved_count = 0;
                                
                                for (int i = 0; i < broj_vozila; i++) {
                                    if (vozila[i].status == 0 && vozila[i].reserved_by_user_id == user_id) {
                                        reserved_vehicles[reserved_count] = vozila[i];
                                        reserved_count++;
                                    }
                                }
                                
                                // Send success response first
                                if(send_message(sock, "# CHECKSTATUS SUCCES #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                                
                                // Then send the table data with reserved vehicles
                                send_tabela(sock, reserved_vehicles, reserved_count);
                            } else {
                                if(send_message(sock, "# CHECKSTATUS ERROR #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            }
                            
                            free(user_id_str);
                        }
                        continue;
                        
                    case CMD_RESERVE:
                        if (broj_rijeci < 3){
                            if(send_message(sock, "# RESERVE ERROR 304 #") < 0) {
                                printf("Thread %d: Send failed\n", thread_index);
                                break;
                            }
                        }else{
                            char *user_id_str = remove_brackets(komanda[1]);
                            char *car_id_str = remove_brackets(komanda[2]);
                            int user_id = atoi(user_id_str);
                            int car_id = atoi(car_id_str);
                            
                            // Check if user exists
                            int user_found = 0;
                            for (int i = 0; i < broj_korisnika; i++) {
                                if (korisnici[i].id == user_id) {
                                    user_found = 1;
                                    break;
                                }
                            }
                            
                            if (!user_found) {
                                if(send_message(sock, "# RESERVE ERROR 305 #") < 0) {
                                    printf("Thread %d: Send failed\n", thread_index);
                                    break;
                                }
                            } else {
                                // Find the vehicle
                                int vehicle_found = 0;
                                int vehicle_index = -1;
                                
                                for (int i = 0; i < broj_vozila; i++) {
                                    if (vozila[i].id == car_id) {
                                        vehicle_found = 1;
                                        vehicle_index = i;
                                        break;
                                    }
                                }
                                
                                if (!vehicle_found) {
                                    // Vehicle doesn't exist
                                    if(send_message(sock, "# RESERVE ERROR 301 #") < 0) {
                                        printf("Thread %d: Send failed\n", thread_index);
                                        break;
                                    }
                                } else if (vozila[vehicle_index].status == 0) {
                                    // Vehicle is already reserved
                                    if(send_message(sock, "# RESERVE ERROR 302 #") < 0) {
                                        printf("Thread %d: Send failed\n", thread_index);
                                        break;
                                    }
                                } else {
                                    // Check if user already has a reservation
                                    int user_has_reservation = 0;
                                    for (int i = 0; i < broj_vozila; i++) {
                                        if (vozila[i].status == 0 && vozila[i].reserved_by_user_id == user_id) {
                                            user_has_reservation = 1;
                                            break;
                                        }
                                    }
                                    
                                    if (user_has_reservation) {
                                        // User already has a reservation
                                        if(send_message(sock, "# RESERVE ERROR 303 #") < 0) {
                                            printf("Thread %d: Send failed\n", thread_index);
                                            break;
                                        }
                                    } else {
                                        // Reserve the vehicle
                                        vozila[vehicle_index].status = 0; // Mark as reserved
                                        vozila[vehicle_index].reserved_by_user_id = user_id;
                                        
                                        // Send success response
                                        if(send_message(sock, "# RESERVE SUCCES #") < 0) {
                                            printf("Thread %d: Send failed\n", thread_index);
                                            break;
                                        }
                                    }
                                }
                            }
                            
                            free(user_id_str);
                            free(car_id_str);
                        }
                        continue;
                        
                    case CMD_UNKNOWN:
                    default:
                        printf("Thread %d: Unknown command '%s'\n", thread_index, komanda[0]);
                        snprintf(response, sizeof(response), "SERVER: Unknown command '%s'", komanda[0]);
                        break;
                }
            } else {
                snprintf(response, sizeof(response), "SERVER: Empty command received");
            }
            
            // Send response to client
            if(send_message(sock, response) < 0) {
                printf("Thread %d: Send failed\n", thread_index);
                break;
            }
            printf("Thread %d: Sent response for command '%s'\n", thread_index, broj_rijeci > 0 ? komanda[0] : "empty");
        }
        
        // Clear the buffer for next message
        memset(client_message, 0, sizeof(client_message));
    }

    // Handle disconnection cases
    if(read_size == 0)
    {
        printf("Client disconnected from thread %d\n", thread_index);
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        printf("Thread %d: recv failed\n", thread_index);
        perror("recv failed");
    }

    // Close socket
    close(sock);
    
    // Mark thread as free
    pthread_mutex_lock(&status_mutex);
    thread_status[thread_index] = 0;
    printf("Thread %d marked as free\n", thread_index);
    pthread_mutex_unlock(&status_mutex);
    
    // Free the parameter array
    free(param);
    
    return NULL;
}