
#ifndef WEB_SERVER_UTILS_H
#define WEB_SERVER_UTILS_H


#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <asm/errno.h>
#include <errno.h>
#include <asm/ioctls.h>
#include <stropts.h>
#include <arpa/inet.h>

#define DIM_HEADER 300

 char* HTML_400 = "<!DOCTYPE HTML> <html><head><title>400 Bad Request</title></head><body><h1>Bad Request</h1><p>Your browser sent a request that this server could not understand.</p><p>The request line contained invalid characters following the protocol string.</p></body></html>";
 char* HTML_INDEX = "<!DOCTYPE HTML> <html><head><title>Content Adaptation</title></head><body><h1>Content Adaptation: Home Page</h1><p><a href=\"/wizard.jpg\">wizard.jpg</a></p></body></html>";

void exit_on_error (int condition, const char * message)
{
    if (condition) {
        perror (message) ;
        exit (EXIT_FAILURE) ;
    }
}

int readn(int fd, char *buf, int n) {

    int nread;

    while(1) {
        nread = recv(fd, buf, n, 0);
        if (nread != 0) break;
        printf("attesa\n");
        sleep(1);
    }
    printf("byte letti = %i", nread);
    if(errno == EWOULDBLOCK) {
        errno=0;
        nread=  0;
    }
    if (errno == EINTR || nread < 0) {
        nread= -1;
    }
    for(int i = 1; i < n; i++) {
        if (buf[i-1] == '\n' && buf[i] == '\n') {
            buf[i+1] = '\0';
            nread = strlen(buf);
            break;
        }
    }
    return nread;
}

ssize_t writen(int fd, const void *buf, size_t n) {
    ssize_t nwritten;
    nwritten = write(fd, buf, n);
    if (errno == EINTR) {
        nwritten = -1;
    }
    return nwritten;
}

#endif //WEB_SERVER_UTILS_H
