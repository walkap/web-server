
#ifndef WEB_SERVER_UTILS_H
#define WEB_SERVER_UTILS_H


#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

void exit_on_error (int condition, const char * message)
{
    if (condition) {
        perror (message) ;
        exit (EXIT_FAILURE) ;
    }
}

#endif //WEB_SERVER_UTILS_H
