
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
#include <ctype.h>

#define DIM_HEADER 300

//TODO html should be in a proper html page

char *HTML_400 = "<!DOCTYPE HTML> <html><head><title>400 Bad Request</title></head><body><h1>Bad Request</h1><p>Your browser sent a request that this server could not understand.</p><p>The request line contained invalid characters following the protocol string.</p></body></html>";
char *HTML_INDEX = "<!DOCTYPE HTML> <html><head><title>Content Adaptation</title></head><body><h1>Content Adaptation: Home Page</h1><p><a href=\"/wizard.jpg\">wizard.jpg</a></p></body></html>";

void exit_on_error (int condition, const char * message)
{
    if (condition) {
        perror (message) ;
        exit (EXIT_FAILURE) ;
    }
}

int readn(int conn, char *ptr, int n) {

    ssize_t readn;
    size_t nleft;

    errno = 0;
    int r = 0;

    nleft = n;


    while (nleft > 0) {
        fflush(stdout);
        if ((readn = recv(conn, ptr, nleft, 0)) > 0) {
            nleft -= readn;
            ptr += readn;
            if (*(ptr - 2) == '\r' && *(ptr - 1) == '\n') {
                fflush(stdout);
                r=1;
                break;
            } else continue;

        } else if (errno == EWOULDBLOCK || readn == 0) {
            r=0;
            break;
        }
    }
    return r;


}

ssize_t writen(int fd, const void *buf, size_t n) {
    ssize_t nwritten;
    nwritten = write(fd, buf, n);
    if (errno == EINTR) {
        nwritten = -1;
    }
    return nwritten;
}


/**
 * This function returns the current directory path
 * as a char pointer
 * @return char *
 */
char *current_dir() {
    char buffer[1024];
    char *current_dir = getcwd(buffer, sizeof(buffer));
    return current_dir;
}

/**
 * This function concatenates two strings and returns a char pointer
 * to the complete string
 * @param s1 - the first string will at the start
 * @param s2 - the second string appended to the first one
 * @return char *
 */
char *concat(char *s1, char *s2) {

    char *target;

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    target = malloc(len1 + len2 + 1);

    if (target == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(target, s1, len1);
    memcpy(target + len1, s2, len2 + 1);

    return target;
}

char *toString(size_t size){
    char *ssize = "";
    sprintf(ssize, "%d", (int) size);
    return ssize;
}


#endif //WEB_SERVER_UTILS_H