#include "server.h"

int main(int argc, char *argv[])
{
    int listensd;

    if (argc > 1) {
        perror("invalid arguments");
        exit(EXIT_FAILURE);
    }

    listensd = init_server();
    run_server(&listensd);

}
