#include "server.h"

/**
 * Close the connection
 * @param pt
 */
void close_connection(struct thread_data *pt) {
    int rv;
    rv = close(pt->sd);
    exit_on_error(rv < 0, "error in close");
    pt->sd = -1;
    pthread_exit(NULL);
}

/**
 * Handle a request from client
 * @param arg
 */
void handle_request(void *arg) {
    struct thread_data *pt = (struct thread_data *) arg;
    int conn = pt->sd;
    int rv;
    int alive;
    rv = pthread_detach(pt->thread);
    exit_on_error(rv != 0, "error in pthread_create");

    ssize_t b_read;
    char buff[MAXLINE] = {0};
    b_read = readn(conn, buff, MAXLINE);
    if (b_read == 0) {

        close_connection(pt);

    }
    printf("\nConnection accepted\n");
    printf("\n%s\n", buff);
    fflush(stdout);

    alive = set_response(buff, conn);

    if (alive) {

        int rv;
        struct timeval tv;
        tv.tv_sec = 10;

        rv = setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
        exit_on_error(rv < 0, "error in setsockopt");

        for (;;) {

            char buff[MAXLINE];
            int r;
            r = readn(conn, buff, MAXLINE);

            if (r) {

                printf("\n%s\n", buff);
                fflush(stdout);

                set_response(buff, conn);
                continue;
            }

            break;
        }
    }

    printf("\nConnection closed\n");
    fflush(stdout);
    close_connection(pt);
}

/**
 * Set the socket address
 * @param ad
 */
void set_addr(struct sockaddr_in *ad) {
    memset((void *) ad, 0, sizeof(ad));
    ad->sin_family = AF_INET;
    ad->sin_port = htons(PORT);
    ad->sin_addr.s_addr = htonl(INADDR_ANY);
}

/**
 * Server initialization
 * @return
 */
int init_server() {
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

/**
 * Look for idle threads
 * @param pt
 * @return
 */
int find_free_thread(struct thread_data *pt) {
    int c = 0;
    while (pt[c].sd != -1) {
        if (c == MAXCONN) {
            return -1;
        }
        c++;
    }
    return c;
}

/**
 * Run the server
 * @param listensd
 */
void run_server(int *listensd) {
    int l = *listensd;
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    int rv;
    int slot;

    struct thread_data *td = alloc_thread_data();

    printf("Server started at http://localhost:8000/index.html\n");
    printf("------Waiting for requests------\n");
    fflush(stdout);

    for (;;) {
        int connsd = accept(l, (struct sockaddr *) &client, &len);
        exit_on_error(connsd < 0, "error in  accept");

        slot = find_free_thread(td);
        exit_on_error(slot == -1, "max num of connections reached");

        td[slot].sd = connsd;


        rv = pthread_create(&(td[slot].thread), NULL, (void *) handle_request, (void *) &td[slot]);
        exit_on_error(rv != 0, "error in pthread_create");
    }
}

int main(int argc, char *argv[])
{
    (void)argv[0];
    int listensd;

    if (argc > 1) {
        perror("invalid arguments");
        exit(EXIT_FAILURE);
    }

    listensd = init_server();
    run_server(&listensd);
}
