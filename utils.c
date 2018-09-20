#include "utils.h"

void exit_on_error(int condition, const char *message) {
    if (condition) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

/**
 * When timeout expires ERRNO is set to EWOULDBLOCK
 * @param conn
 * @param ptr
 * @param n_bytes
 * @return int
 */
int readn(int conn, char *ptr, int n_bytes) {
    ssize_t readn;
    size_t nleft;
    errno = 0;
    int r = 0;

    nleft = (size_t) n_bytes;

    while (nleft > 0) {
        fflush(stdout);
        if ((readn = recv(conn, ptr, nleft, 0)) > 0) {
            nleft -= readn;
            ptr += readn;
            if (*(ptr - 2) == '\r' && *(ptr - 1) == '\n') {
                fflush(stdout);
                r = 1;
                break;
            } else continue;

        } else if (errno == EWOULDBLOCK || readn == 0) {
            r = 0;
            break;
        }
    }
    return r;
}

ssize_t writen(int fd, const void *buf, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = buf;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if ((nwritten < 0) && (errno == EINTR)) nwritten = 0; else return (-1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (nleft);
}