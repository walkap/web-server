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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <ImageMagick-7/MagickWand/MagickWand.h>

#define DIM_HEADER 300
#define N_PAGES 100
#define BUFFDIM 2000000 //TODO added a zero it should be dynamic

#define IMAGE_DIR "../www/images"
#define FILE_DIR "../www"

char *CACHE;
pthread_mutex_t mutex;

void exit_on_error (int condition, const char * message);
int readn(int conn, char *ptr, int n);
ssize_t writen(int fd, const void *buf, size_t n);
off_t get_file_size(int fd);

#endif //WEB_SERVER_UTILS_H