#include "cache.h"

// comment
/* control */
/** lack */

// find the first empty byte and return the numbers of bytes from cache start (-1 for error)
int find_empty_cell(char *cache, struct memory_cell **cell){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    int sum = 0;

    struct memory_cell* cells = (struct memory_cell*) cache;

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memory_cell)){

        if(*(cells -> title) == '\0'){
            *cell = cells;
            //printf("control find_empty_cell sum :%d\n", sum);
            return(sum);

        }

        //printf("control find_empty_cell i: %d\n", i);

        cache += i;
        cells = (struct memory_cell*) cache;

        //printf("control find_empty_cell cache: %p\n", cells);

        sum += i;

        //printf("control find_empty_cell sum :%d\n", sum);

    }

    return -1;

}

// find the less useful cache item and return the numbers of bytes from cache start (-1 for error)
unsigned int find_less_useful(char *cache, size_t fileLength, struct memory_cell **cell){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    int sum = 0;

    int positionLessUseful = 0;

    struct memory_cell* cells = (struct memory_cell*) cache;

    struct memory_cell* lessUseful = cells;

    if((cells -> next) < fileLength){
        positionLessUseful = -1;

    }

    //printf("*** FINDLESSFUL ***\n");

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memory_cell)){

        cache += i;
        cells = (struct memory_cell*) cache;

        sum += i;

        //printf("control find lessUseful %p \ncell seconds: %ld\nlessUseful seconds: %ld\n", cache, cells -> seconds, lessUseful -> seconds);

        if(*(cells -> title) != '\0' && (cells -> next) >= fileLength && cells -> seconds < lessUseful -> seconds){

            lessUseful = cells;
            positionLessUseful = sum;

        }

        //printf("control find_less_useful i: %d\n", i);

        //printf("control find_less_useful cache: %p\n", cells);

        //printf("control find_less_useful sum :%d\n", sum);

    }

    if(positionLessUseful != -1){
        *cell = lessUseful;

    }

    return positionLessUseful;

}

/** find less useful with bigger space */

// insert one item in cache and return 1 for success (-1 for error)
int cache_insert(char *cache, void *file, size_t file_length, char *name, double quality_image, size_t image_length,
                 size_t image_width){

    // generate cellMem
    struct memory_cell *cell;

    cell = (struct memory_cell*) malloc(sizeof(struct memory_cell));

    strcpy(cell->title, name);

    printf("hey");

    cell->length = file_length;

    cell->next = file_length;

    cell->quality = quality_image;

    cell->seconds = time(NULL);

    cell->imageLength = image_length;

    cell->imageWidth = image_width;

    struct memory_cell* emptyCell;

    // check for empty cell (change)
    int numberByte = find_empty_cell(cache, &emptyCell);
    printf("control find_empty_cell numerByte: %d\n", numberByte);

    // find the cell to eliminate if the cell are full
    if(numberByte == -1){

        // find the less useful cache cell
        numberByte = find_less_useful(cache, file_length, &emptyCell);
        printf("control find_less_useful numerByte: %d\n", numberByte);

        if(numberByte == -1){

            fprintf(stderr, "cache full and not useful find\n");
            return -1;

        }

        cell->next = ((struct memory_cell*) (cache + numberByte)) -> next;

    }

    // insert pointer in cell
    cell->pointer = cache + numberByte;

    printf("control cache_insert cell.pointer : %p\n", cell->pointer);

    printf("control cache_insert cell.seconds: %ld\n", cell->seconds);

    // insert struct in cache
    memcpy(cell->pointer, &cell, sizeof(struct memory_cell));

    // insert image in cache
    memcpy(cell->pointer + sizeof(struct memory_cell), file, file_length);

    printf("item insert with success\n");

    // return value
    return 1;

}

// check if one item is in the cache
int cache_check(char *cache, struct memory_cell **cell, char *name, double qualityImage,
                unsigned int imageLength, unsigned int imageWidth){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    struct memory_cell* cells = (struct memory_cell*) cache;

    if(strcmp(cells -> title, "") == 0){

        return -1;
    }

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memory_cell)){

        printf("control cache_check cell title: %s\n", cells -> title);
        printf("control cache_check title: %s\n", name);
        printf("control cache_check cell quality: %u\n", cells -> quality);
        printf("control cache_check cell quality: %u\n", qualityImage);
        printf("control cache_check cache: %p\n", cells);

        cache += i;
        cells = (struct memory_cell*) cache;

        if(strcmp(cells -> title, name) == 0 && cells -> quality == qualityImage && cells -> imageLength == imageLength && cells -> imageWidth == imageWidth){

            //sleep(1);

            cells -> seconds = time(NULL);
            *cell = cells;
            return(i);

        }

        //printf("control cache_check i: %d\n", i);

    }

    return -1;

}

int cache_initialize(size_t numb, char **cache){

    unsigned int length = (unsigned int)numb *  getpagesize();

    // create cache pointer and calloc
    *cache = (char*) calloc(1, length);
    if(*cache == NULL){
        return -1;

    }

    **((unsigned int**) cache) = length - sizeof(unsigned int);

    *cache += sizeof(unsigned int);

    printf("cache :%p\n", cache);

    return 1;

}