#ifndef WEB_SERVER_CACHE_H
#define WEB_SERVER_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// comment
/* control */
/** lack */

struct memCell {

    // image title
    char title[20];

    // quality of the image
    double quality;

    int height;

    int width;

    // start in cache
    char* pointer;

    // length in cache
    unsigned int length;

    // time of last hit
    time_t seconds;

};


int cacheInsert(char* cache, unsigned int cacheLength, FILE *file, char* name, double qualityImage,
                int height, int width);
int checkMemory(char* cache, unsigned int cacheLength, struct memCell** cell, char* name, double qualityImage,
                int height, int width);
#endif //WEB_SERVER_CACHE_H
