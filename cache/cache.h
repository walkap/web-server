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

struct memCell;

int cacheInsert(char* cache, FILE *file, char* name, unsigned int qualityImage);

int checkMemory(char* cache, struct memCell** cell, char* name, unsigned int qualityImage);

#endif //WEB_SERVER_CACHE_H
