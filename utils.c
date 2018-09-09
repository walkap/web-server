#include "utils.h"

void exit_on_error(int condition, const char *message) {
    if (condition) {
        perror(message);
        exit(EXIT_FAILURE);
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
        if ((readn = recv(conn, ptr, nleft, 0)) >
            0) { //TODO This should be used as infinite loop, wait till something arrive
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

ssize_t writen(int fd, const void *buf, size_t n)
{
    size_t nleft;

    ssize_t nwritten;
    const char *ptr;
    ptr = buf;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if ((nwritten < 0) && (errno == EINTR)) nwritten = 0; else return(-1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    }


    return(nleft);
}

/**
 * Returns the size of the content of the file
 * @param fd file descriptor
 * @return  off_t the offset of the file
 */
//TODO this could be shared with the whole project
off_t get_file_size(int fd) {
    int stat;
    struct stat info;

    //Get file info and save into the struct created
    stat = fstat(fd, &info);
    if (stat == -1) {
        perror("fstat()");
        exit(EXIT_FAILURE);
    }
    //Get the result
    off_t size = info.st_size;

    return size;
}