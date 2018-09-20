#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "../utils.h"
#include "../http_parsing/response_setter.h"

#define PORT 8000
#define BACKLOG 10
#define MAXLINE 30000
#define MAXCONN 1000
#define CACHE_ACTIVE 0

#if CACHE_ACTIVE
#define N_PAGES 10000
char *CACHE;
pthread_mutex_t mutex;
#endif

struct thread_data {
    pthread_t thread;
    int sd;
};

int init_server();
void run_server(int *listen_sd);

#endif //WEB_SERVER_H