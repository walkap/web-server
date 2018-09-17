#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "../utils.h"
#include "../http_parsing/response_setter.h"

#define PORT 8000
#define BACKLOG 10
#define MAXLINE 30000
#define MAXCONN 1000

struct thread_data {
    pthread_t thread;
    int sd;
    /*char *addr;
    int port;*/
};


//TODO why we need to function to call to run a server?
int init_server();
void run_server(int *listensd);

#endif //WEB_SERVER_H