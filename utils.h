
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

char *HTTP_404 = "HTTP/1.1 404 Not Found\nContent-Type: text/html; charset=iso-8859-1\nContent-Length: 141\nConnection: Closed\n\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> <html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL  was not found on this server.</p></body></html>";
char *HTTP_400 = "HTTP/1.1 400 Bad Request\ntext/html; charset=iso-8859-1\nContent-Length: 246\nConnection: Closed\n\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> <html><head><title>400 Bad Request</title></head><body><h1>Bad Request</h1><p>Your browser sent a request that this server could not understand.</p><p>The request line contained invalid characters following the protocol string.</p></body></html>";
char *HTTP_INDEX = "HTTP/1.1 200 OK\nContent-Length: 205\nContent-Type: text/html\nConnection: keep-alive\n\n<!DOCTYPE HTML PUBLIC \\\"-//IETF//DTD HTML 2.0//EN\\\"> <html><head><title>Content Adaptation</title></head><body><h1>Content Adaptation: Home Page</h1><p>Inserire link delle immagini //TODO</p></body></html>";
char *HTTP_404_1 = "HTTP/1.1 404 Not Found\nConnection: Closed\n\n";

void exit_on_error(int condition, const char *message) {
    if (condition) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

int readn(int fd, char *buf, int n) {

    int nread;

    while (1) {
        nread = recv(fd, buf, n, 0);
        if (nread != 0) break;
        printf("attesa\n");
        sleep(1);
    }
    printf("byte letti = %i", nread);
    if (errno == EWOULDBLOCK) {
        errno = 0;
        nread = 0;
    }
    if (errno == EINTR || nread < 0) {
        nread = -1;
    }
    for (int i = 1; i < n; i++) {
        if (buf[i - 1] == '\n' && buf[i] == '\n') {
            buf[i + 1] = '\0';
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
 * @return *char
 */
char *concat(char * s1, char *s2){

    char *target;

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    target = malloc(len1 + len2 + 1);

    if(target == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(target, s1, len1);
    memcpy(target + len1, s2, len2 +1);

    return target;
}


#endif //WEB_SERVER_UTILS_H
