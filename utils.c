#include "utils.h"

char *HTML_400 = "<!DOCTYPE HTML> <html><head><title>400 Bad Request</title></head><body><h1>Bad Request</h1><p>Your browser sent a request that this server could not understand.</p><p>The request line contained invalid characters following the protocol string.</p></body></html>";
char *HTML_INDEX = "<!DOCTYPE HTML> <html><head><title>Content Adaptation</title></head><body><h1>Content Adaptation: Home Page</h1><p><a href=\"/wizard.jpg\">wizard.jpg</a></p></body></html>";

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

//TODO what is this for?
ssize_t writen(int fd, const void *buf, size_t n) {
    ssize_t nwritten;
    nwritten = write(fd, buf, n);
    if (errno == EINTR) {
        nwritten = -1;
    }
    return nwritten;
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