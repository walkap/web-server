#include "server.h"

/**
 * This function is used to close the socket connection and terminates the thread
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
    int conn = pt->sd, rv, alive;
    ssize_t b_read;
    char buff[MAXLINE] = {0};
    rv = pthread_detach(pt->thread);
    exit_on_error(rv != 0, "error in pthread_create");

    b_read = readn(conn, buff, MAXLINE);
    if (b_read == 0) {
        close_connection(pt);
    }
    printf("\nConnection accepted\n");
    printf("\n%s\n", buff);
    fflush(stdout);

    alive = set_response(buff, conn);

    /*Persistent connections: If the request has the "Connection:keep-alive" header line, the connection is not closed,
     the thread waits for more requests sent via socket. A timeout is set to 10 seconds, if no requests are sent during
     this time, the connection is closed.*/
    if (alive) {
        struct timeval tv;
        tv.tv_sec = 10;

        setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

        for (;;) {
            b_read = readn(conn, buff, MAXLINE);
            if (b_read != 0) {

                set_response(buff, conn);
                continue;
            }
            break;
        }
    }

    printf("\nConnection closed\n");
    close_connection(pt);
}

/**
 * This function is used to initialize socket
 * @param address
 */
void set_address(struct sockaddr_in *address) {
    memset((void *) address, 0, sizeof(address));
    address->sin_family = AF_INET;
    address->sin_port = htons(PORT);
    address->sin_addr.s_addr = htonl(INADDR_ANY);
}

/**
 * Server initialization
 * @return int
 */
int init_server() {
    int listensd, rv, tr;
    struct sockaddr_in addr;
    listensd = socket(AF_INET, SOCK_STREAM, 0);
    exit_on_error(listensd < 0, "error in socket");
    tr = 1;
    rv = setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int));
    exit_on_error(rv < 0, "error in setsockopt");
    set_address(&addr);
    rv = bind(listensd, (struct sockaddr *) &addr, sizeof(addr));
    exit_on_error(rv < 0, "error in bind");
    rv = listen(listensd, BACKLOG);
    exit_on_error(rv < 0, "error in listen");

    return listensd;
}

/**
 * Look for idle threads
 * @param pt
 * @return int
 */
int find_free_thread(struct thread_data *pt) {
    int c = 0;
    while (pt[c].sd != -1) {
        if (c == MAXCONN) {
            usleep(100);
            c = 0;
            continue;
        }
        c++;
    }
    return c;
}

/**
 *This function initializes an array of thread_data struct
 * @return
 */
struct thread_data *alloc_thread_data() {
    struct thread_data *td = malloc(sizeof(struct thread_data) * MAXCONN);
    exit_on_error(td == NULL, "error in malloc");

    for (int j = 0; j < MAXCONN; j++) {
        td[j].sd = -1;
    }
    return td;
}

/**
 * Run the server, when a new connection is ready, a new thread is started to handle the request.
 * @param listen_sd
 */
void run_server(int *listen_sd) {

    int received_value, slot, connection_sd;
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    struct thread_data *td = alloc_thread_data();

    #if CACHE_ACTIVE

    printf("CACHE initialize\n");

    if (cache_initialize(N_PAGES, &CACHE) == -1) {
        fprintf(stdout, "error in cache_initialize\n");
    }

    #endif

    printf("Server started at http://localhost:8000/index.html\n");
    printf("------Waiting for requests------\n");
    fflush(stdout);

    while ((connection_sd = accept(*listen_sd, (struct sockaddr *) &client, &len)) > 0) {
        slot = find_free_thread(td);
        exit_on_error(slot == -1, "max num of connections reached");
        td[slot].sd = connection_sd;
        received_value = pthread_create(&(td[slot].thread), NULL, (void *) handle_request, (void *) &td[slot]);
        exit_on_error(received_value != 0, "error in pthread_create");
    }
}

int main(void) {
    int listen_sd;
    listen_sd = init_server();
    run_server(&listen_sd);
}