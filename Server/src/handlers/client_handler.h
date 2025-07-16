#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <pthread.h>

// Global variables declarations
extern int thread_status[10];
extern pthread_mutex_t status_mutex;

// Function declarations
void *connection_handler(void *param);

#endif