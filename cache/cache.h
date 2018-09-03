#ifndef WEB_SERVER_CACHE_H
#define WEB_SERVER_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

struct memCell {

    // image title
    char title[20];

    // quality of the image
    unsigned int quality;

    // start in buff
    char* pointer;

    // length in buff
    unsigned int length;

    // next item
    unsigned int next;

    // time of last hit
    time_t seconds;

    // image length
    unsigned int imageLength;

    // image width
    unsigned int imageWidth;

};

int initializeCache(size_t numb, char** cache);

int cacheInsert(char* cache, void *file, char* name, unsigned int qualityImage, unsigned int imageLength, unsigned int imageWidth);

int checkMemory(char* cache, struct memCell** cell, char* name, unsigned int qualityImage, unsigned int imageLength, unsigned int imageWidth);

#endif //WEB_SERVER_CACHE_H
