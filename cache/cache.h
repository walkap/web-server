#ifndef WEB_SERVER_CACHE_H
#define WEB_SERVER_CACHE_H

#include "../utils.h"

struct memory_cell {

    // image title
    char title[20];

    // quality of the image
    double quality;

    // start in buff
    char* pointer;

    // length in buff
    size_t length;

    // next item
    size_t next;

    // time of last hit
    time_t seconds;

    // image length
    unsigned int imageLength;

    // image width
    unsigned int imageWidth;

};

int cache_initialize(size_t numb, char **cache);

int cache_insert(char *cache, void *file, size_t file_length, char *name, double quality_image, size_t image_length,
                 size_t image_width);

int cache_check(char *cache, struct memory_cell **cell, char *name, double qualityImage,
                unsigned int imageLength, unsigned int imageWidth);

#endif //WEB_SERVER_CACHE_H
