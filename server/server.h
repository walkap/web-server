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
};

struct thread_data *alloc_thread_data() {
    struct thread_data *td = malloc(sizeof(struct thread_data) * MAXCONN);
    exit_on_error(td == NULL, "error in malloc");

    for (int j = 0; j < MAXCONN; j++) {
        td[j].sd = -1;
    }
    return td;
}

//TODO why we need to function to call to run a server?
int init_server();
void run_server(int *listensd);

#endif //WEB_SERVER_H