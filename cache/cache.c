#include "cache.h"

// comment
/* control */
/** lack */

// definition of struct
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

// find the first empty byte and return the numbers of bytes from cache start (-1 for error)
int findEmptyCell(char* cache, struct memCell** cell){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    int sum = 0;

    struct memCell* cells = (struct memCell*) cache;

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memCell)){

        if(*(cells -> title) == '\0'){
            *cell = cells;
            //printf("control findEmptyCell sum :%d\n", sum);
            return(sum);

        }

        //printf("control findEmptyCell i: %d\n", i);

        cache += i;
        cells = (struct memCell*) cache;

        //printf("control findEmptyCell cache: %p\n", cells);

        sum += i;

        //printf("control findEmptyCell sum :%d\n", sum);

    }

    return -1;

}

// find the less useful cache item and return the numbers of bytes from cache start (-1 for error)
unsigned int findLessUseful(char* cache, size_t fileLength, struct memCell** cell){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    int sum = 0;

    int positionLessUseful = 0;

    struct memCell* cells = (struct memCell*) cache;

    struct memCell* lessUseful = cells;

    if((cells -> next) < fileLength){
        positionLessUseful = -1;

    }

    printf("*** FINDLESSFUL ***\n");

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memCell)){

        cache += i;
        cells = (struct memCell*) cache;

        sum += i;

        printf("control find lessUseful %p \ncell seconds: %ld\nlessUseful seconds: %ld\n", cache, cells -> seconds, lessUseful -> seconds);

        if(*(cells -> title) != '\0' && (cells -> next) >= fileLength && cells -> seconds < lessUseful -> seconds){

            printf("*** HEY ***\n");

            lessUseful = cells;
            positionLessUseful = sum;

        }

        //printf("control findLessUseful i: %d\n", i);

        //printf("control findLessUseful cache: %p\n", cells);

        //printf("control findLessUseful sum :%d\n", sum);

    }

    if(positionLessUseful != -1){
        *cell = lessUseful;

    }

    return positionLessUseful;

}

/** find less useful with bigger space */

// insert one item in cache and return 1 for success (-1 for error)
int cacheInsert(char* cache, void *file, char* name, unsigned int qualityImage, unsigned int imageLength, unsigned int imageWidth){

    // generate cellMem
    struct memCell cell;

    // fill cellMem
    fseek(file, 0, SEEK_END);
    unsigned long fileLength = ftell(file);
    fseek(file, 0, SEEK_SET);

    strcpy(cell.title, name);

    cell.length = fileLength;

    cell.next = fileLength;

    cell.quality = qualityImage;

    cell.seconds = time(NULL);

    cell.imageLength = imageLength;

    cell.imageWidth = imageWidth;

    struct memCell* emptyCell;

    // check for empty cell (change)
    int numberByte = findEmptyCell(cache, &emptyCell);
    printf("control findEmptyCell numerByte: %d\n", numberByte);

    // find the cell to eliminate if the cell are full
    if(numberByte == -1){

        // find the less useful cache cell
        numberByte = findLessUseful(cache, fileLength, &emptyCell);
        printf("control findLessUseful numerByte: %d\n", numberByte);

        if(numberByte == -1){

            fprintf(stderr, "cache full and not useful find\n");
            exit(EXIT_FAILURE);

        }

        cell.next = ((struct memCell*) (cache + numberByte)) -> next;

    }

    // insert pointer in cell
    cell.pointer = cache + numberByte;

    printf("control cacheInsert cell.pointer : %p\n", cell.pointer);

    printf("control cacheInsert cell.seconds: %ld\n", cell.seconds);

    // insert struct in cache
    memcpy(cell.pointer, &cell, sizeof(struct memCell));

    // insert image in cache
    memcpy(cell.pointer + sizeof(struct memCell), file, fileLength);

    printf("item insert with success\n");

    // return value
    return 1;

}

// check if one item is in the cache
int checkMemory(char* cache, struct memCell** cell, char* name, unsigned int qualityImage){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    struct memCell* cells = (struct memCell*) cache;

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memCell)){

        //printf("control checkMemory cell title: %s\n", cells -> title);
        //printf("control checkMemory title: %s\n", name);
        //printf("control checkMemory cell quality: %u\n", cells -> quality);
        //printf("control checkMemory cell quality: %u\n", qualityImage);
        //printf("control checkMemory cache: %p\n", cells);

        cache += i;
        cells = (struct memCell*) cache;

        if(strcmp(cells -> title, name) == 0 && cells -> quality == qualityImage){

            //sleep(1);

            cells -> seconds = time(NULL);
            *cell = cells;
            return(i);

        }

        //printf("control checkMemory i: %d\n", i);

    }

    return -1;

}