
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

void *handle_request(void *arg)
{
    struct thread_data *pt = (struct thread_data *)arg;
    int conn = pt->sd;

    ssize_t b_read, b_written;
    char *buff;
    char *http_response;
    int rv;
    buff = malloc(MAXLINE);
    exit_on_error(buff == NULL, "error in malloc");

    b_read = read(conn, buff, MAXLINE);
    exit_on_error(b_read == -1, "error in read");

    printf("%s\n", buff);

    http_response = set_response(buff);

    b_written = write(conn, http_response, strlen(http_response));
    exit_on_error(b_written == -1, "error in write");

    rv = close(conn);
    exit_on_error(rv < 0, "error in close");
    pt->sd=-1;
    printf("------Waiting for requests------\n");
    pthread_exit(NULL);

}

void set_addr(struct sockaddr_in* ad)
{
    memset((void *)ad, 0, sizeof(ad));
    ad->sin_family = AF_INET;
    ad->sin_port = htons(PORT);
    ad->sin_addr.s_addr = htonl(INADDR_ANY);

}

int init_server()
{
    int listensd;
    int rv;
    struct sockaddr_in addr;
    listensd = socket(AF_INET, SOCK_STREAM, 0);
    exit_on_error(listensd < 0, "error in socket");
    set_addr(&addr);
    rv = bind(listensd, (struct sockaddr *) &addr, sizeof(addr));
    exit_on_error(rv < 0, "error in bind");
    rv = listen(listensd, BACKLOG);
    exit_on_error(rv < 0, "error in listen");
    int tr = 1;
    rv = setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int));
    exit_on_error(rv < 0, "error in setsockopt");

    return listensd;

}

int find_free_thread(struct thread_data* pt)
{
    int c = 0;
    while(pt[c].sd!=-1) {
        if (c == MAXCONN){
            return -1;
        }
        c++;
    }
    return c;
}

void run_server(int *listensd)
{
    int connsd;
    int l=*listensd;
    int rv;
    int slot;

    struct thread_data *td = malloc(sizeof(struct thread_data)*MAXCONN);
    exit_on_error(td==NULL, "error in malloc");

    for (int j = 0; j < MAXCONN; j++) {
        td[j].sd = -1;
    }

    printf("Server started at localhost:8000/index.html\n");

    printf("------Waiting for requests------\n");

    for(;;) {

        connsd = accept(l, (struct sockaddr*) NULL, NULL);
        exit_on_error(connsd < 0, "error in  accept");

        slot = find_free_thread(td);
        exit_on_error(slot ==-1, "max num of connections reached");


        td[slot].sd=connsd;

        rv = pthread_create(&(td[slot].thread), NULL, handle_request,  (void *)&td[slot]);
        exit_on_error(rv != 0, "error in pthread_create");


  }
}

#endif //WEB_SERVER_H
