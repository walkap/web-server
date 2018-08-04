#include "server.h"

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
