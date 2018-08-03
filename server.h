
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "utils.h"

#define PORT 8000
#define BACKLOG 10
#define MAXLINE 30000

char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

void *handle_request(void *arg)
{
    int *conn = (int *)arg;
    ssize_t b_read, b_written;

    size_t b_left;
    char *buff;
    int rv;

    buff = malloc(MAXLINE);
    b_left = MAXLINE;
    exit_on_error(buff == NULL, "error in malloc");

    b_read = read(*conn, buff, b_left);
    exit_on_error(b_read == -1, "error in read");

    printf("%s\n", buff);

    b_left = strlen(http_response);
    while ((b_written = write(*conn, http_response, b_left)) > 0) {
        exit_on_error(b_written == -1, "error in write");
        b_left -= b_written;
        http_response += b_written;
    }

    rv = close(*conn);
    exit_on_error(rv < 0, "error in close");

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

    return listensd;

}

void run_server(int *listensd)
{
    int connsd;
    int rv;
    pthread_t thread;


    for(;;) {

        printf("------Waiting for requests------\n");

        connsd = accept(*listensd, (struct sockaddr*) NULL, NULL);
      exit_on_error(connsd < 0, "error in  accept");

      rv = pthread_create(&thread, NULL, handle_request,  &connsd);
      exit_on_error(rv != 0, "error in pthread_create");

  }
}

#endif //WEB_SERVER_H
