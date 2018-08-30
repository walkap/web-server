#include "cache.h"

// comment
/* control */
/** lack */

// definition of struct

// find the first empty byte and return the numbers of bytes from cache start (-1 for error)
int findEmptyCache(char* cache, unsigned int cacheLength, struct memCell** cell){

    int sum = 0;

    struct memCell* cells = (struct memCell*) cache;

    for(unsigned int i = 0; i < cacheLength ; i += (cells -> length) + sizeof(struct memCell)){
        if(*(cells -> title) == '\0'){
            *cell = cells;
            /* printf("control sum :%d\n", sum); */
            return(sum);

        }

        /* printf("control findEmptyCell i: %d\n", i); */

        cache += i;
        cells = (struct memCell*) cache;

        /* printf("control findEmptyCell cache: %p\n", cells); */

        sum += i;

        /* printf("control findEmptyCell sum :%d\n", sum); */

    }

    return -1;

}

// find the less useful cache item and return the numbers of bytes from cache start (-1 for error)
int findLessUseful(char* cache, unsigned int cacheLength, size_t fileLength, struct memCell** cell){

    int sum = 0;

    int positionLessUseful = -1;

    struct memCell* cells = (struct memCell*) cache;

    struct memCell* lessUseful = cells;

    for(unsigned int i = 0; i < cacheLength ; i += (cells -> length) + sizeof(struct memCell)){
        if(*(cells -> title) != '\0' && cells -> length == fileLength ){
            if(cells -> seconds > lessUseful -> seconds)
                lessUseful = cells;
            positionLessUseful = sum;

        }

        /* printf("control findLessUseful i: %d\n", i); */

        cache += i;
        cells = (struct memCell*) cache;

        /* printf("control findLessUseful cache: %p\n", cells); */

        sum += i;

        /* printf("control findLessUseful sum :%d\n", sum); */

    }

    if(positionLessUseful != -1){
        *cell = lessUseful;

    }

    return positionLessUseful;

}

/** find less useful with bigger space */

// insert one item in cache and return 1 for success (-1 for error)
int cacheInsert(char* cache, unsigned int cacheLength, FILE *file, char* name, double qualityImage,
                                     int height, int width){

    // generate cellMem
    struct memCell cell;

    // fill cellMem
    fseek(file, 0, SEEK_END);
    unsigned long fileLength = ftell(file);
    fseek(file, 0, SEEK_SET);

    strcpy(cell.title, name);

    cell.length = fileLength;

    cell.width = width;

    cell.height = height;

    cell.quality = qualityImage;

    cell.seconds = time(NULL);

    struct memCell* emptyCell;

    // check for empty space
    int numberByte = findEmptyCache(cache, cacheLength, &emptyCell);

    // find the cell to eliminate if the cell are full
    if(numberByte == -1){

        // find the less useful cache slot
        numberByte = findLessUseful(cache, cacheLength, fileLength, &emptyCell);

        if(numberByte == -1){

            /* fprintf(stderr, "cache full and not useful find\n"); */
            return -1;

        }

    }

    // insert pointer in cell
    cell.pointer = cache + numberByte;

    /* printf("control cell.pointer :%p\n", cell.pointer); */

    // insert struct in cache
    memcpy(cell.pointer, &cell, sizeof(struct memCell));

    // insert image in cache
    memcpy(cell.pointer + sizeof(struct memCell), file, fileLength);

    // return value
    return 1;

}

// check if one item is in the cache
int checkMemory(char* cache, unsigned int cacheLength, struct memCell** cell, char* name, double qualityImage,
        int height, int width){

    int sum = 0;

    struct memCell* cells = (struct memCell*) cache;

    for(unsigned int i = 0; i < cacheLength ; i += (cells -> length) + sizeof(struct memCell)){
        if(*(cells -> title) != '\0' && *(cells -> title) == *name && cells -> quality == qualityImage
                                                                      && cells -> height == height && cells->width == width){

            cells -> seconds = time(NULL);
            *cell = cells;
            return(sum);

        }

        /* printf("control checkMemory i: %d\n", i); */

        cache += i;
/*
        cells = (struct memCell*) cache;
*/

        /* printf("control checkMemory cache: %p\n", cells); */

        sum += i;

        /* printf("control checkMemory sum :%d\n", sum); */

    }

    return -1;

}