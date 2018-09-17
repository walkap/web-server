#include "server.h"

/**
 * This method is used to close the socket connection and terminates the thread
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

    printf("TID: %d\n", (int) pthread_self());
    struct thread_data *pt = (struct thread_data *) arg;
    int conn = pt->sd, rv, alive;
    ssize_t b_read;
    char *buff = malloc(MAXLINE);
    exit_on_error(buff == NULL, "error in malloc");

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

        rv = setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
        //exit_on_error(rv < 0, "error in setsockopt");

        for (;;) {
            b_read = readn(conn, buff, MAXLINE);
            if (b_read != 0) {

                set_response(buff, conn);
                continue;
            }
            break;
        }
    }

     /*while( recv(conn, buff, MAXLINE, 0) > 0 ) {
         printf("INSIDE THREAD %s", buff);
         set_response(buff, conn);
     }*/

         printf("\nConnection closed\n");
         free(buff);
         close_connection(pt);
}

/**
 * This method is used to initialize socket
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
    int listensd, rv, tr;
    struct sockaddr_in addr;
    listensd = socket(AF_INET, SOCK_STREAM, 0);
    exit_on_error(listensd < 0, "error in socket");
    set_addr(&addr);
    rv = bind(listensd, (struct sockaddr *) &addr, sizeof(addr));
    exit_on_error(rv < 0, "error in bind");
    rv = listen(listensd, BACKLOG);
    exit_on_error(rv < 0, "error in listen");
    tr = 1;
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
            sleep(1);
            c = 0;
            continue;
        }
        c++;
    }
    return c;
}
/**
 *This method initializes an array of thread_data struct
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

/*int thread_exist(struct thread_data *pt, char *addr, int port)
{
    for (int c = 0; c < MAXCONN; c++) {
        if (pt[c].sd != -1 && strcmp(pt[c].addr, addr) == 0) {
            printf("SOCKET ACTIVE port :%d, IP: %s\n", port, addr);
            return pt[c].sd;
        }
    }
    return 0;

}*/

/**
 * Run the server, when a new connection is ready, a new thread is started to handle the request.
 * @param listensd
 */
void run_server(int *listensd) {

    int received_value, slot, connsd;
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    struct thread_data *td = alloc_thread_data();

    if (cache_initialize(N_PAGES, &CACHE) == -1) {
        fprintf(stdout, "error in cache_initialize\n");
    }

    printf("Server started at http://localhost:8000/index.html\n");
    printf("------Waiting for requests------\n");
    fflush(stdout);

    while( (connsd = accept(*listensd, (struct sockaddr *) &client, &len) ) > 0){
        slot = find_free_thread(td);
        exit_on_error(slot == -1, "max num of connections reached");

        td[slot].sd = connsd;

      /*  struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char* str = malloc(INET_ADDRSTRLEN);
        inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
        printf("CLIENT sin port: %d\n", (int) client.sin_port);

        td[slot].addr= str;
        td[slot].port= client.sin_port;

        printf("CLIENT sin addr: %s\n", str);
        int socket;
        if ((socket = thread_exist(td, str, client.sin_port))>0) {
            char *buff = malloc(MAXLINE);
            printf("CONNECTION ALREADY EXISTS\n");
            read(connsd, buff, MAXLINE);
            printf("READING CONNSD %s\n", buff);
            size_t lenght = strlen(buff);
            ssize_t written_bytes = send(socket, buff, lenght, 0);
            printf("WRITTEN BYTES: %d\n", (int) written_bytes);
        }else {
            td[slot].addr= str;
            td[slot].port= client.sin_port;
*/
            received_value = pthread_create(&(td[slot].thread), NULL, (void *) handle_request, (void *) &td[slot]);
            exit_on_error(received_value != 0, "error in pthread_create");
        }
    }

int main(void) {
    int listensd;
    listensd = init_server();
    run_server(&listensd);
}