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

        if(strcmp(cells -> title, "") == 0){
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
int find_less_useful(char *cache, size_t file_length, struct memory_cell **cell){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    int sum = 0;

    int position_less_useful = 0;

    struct memory_cell* cells = (struct memory_cell*) cache;

    struct memory_cell* less_useful = cells;

    if((cells -> next) < file_length){
        position_less_useful = -1;

    }

    //printf("*** FINDLESSFUL ***\n");

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memory_cell)){

        cache += i;
        cells = (struct memory_cell*) cache;

        sum += i;

        //printf("control find less_useful %p \ncell seconds: %ld\nless_useful seconds: %ld\n", cache, cells -> seconds, less_useful -> seconds);

        if(strcmp(cells -> title, "") == 0 && (cells -> next) >= file_length && cells -> seconds < less_useful -> seconds){

            less_useful = cells;
            position_less_useful = sum;

        }

        //printf("control find_less_useful i: %d\n", i);

        //printf("control find_less_useful cache: %p\n", cells);

        //printf("control find_less_useful sum :%d\n", sum);

    }

    if(position_less_useful != -1){
        *cell = less_useful;

    }

    return position_less_useful;

}

/** find less useful with bigger space */

// insert one item in cache and return 1 for success (-1 for error)
int cache_insert(char *cache, void *file, size_t file_length, char *name, double quality_image, size_t image_length,
                 size_t image_width){

    //printf("*** CACHE INSERT ***\n");

    // generate cellMem
    struct memory_cell cell;

    strcpy(cell.title, name);

    cell.length = file_length;

    cell.next = file_length;

    cell.quality = quality_image;

    cell.seconds = time(NULL);

    cell.image_length = image_length;

    cell.image_width = image_width;

    struct memory_cell* empty_cell;

    // check for empty cell (change)
    int bytes_number = find_empty_cell(cache, &empty_cell);
    //printf("control find_empty_cell numerByte: %d\n", bytes_number);

    // find the cell to eliminate if the cell are full
    if(bytes_number == -1){

        // find the less useful cache cell
        bytes_number = find_less_useful(cache, file_length, &empty_cell);
        //printf("control find_less_useful numerByte: %d\n", bytes_number);

        if(bytes_number == -1){

            fprintf(stderr, "cache full and not useful find\n");
            return -1;

        }

        cell.next = ((struct memory_cell*) (cache + bytes_number)) -> next;

    }

    // insert pointer in cell
    cell.pointer = cache + bytes_number;

    //printf("control cache_insert cell.pointer : %p\n", (void*) cell.pointer);
    //printf("control cache_insert cell.seconds: %ld\n", cell.seconds);
    //printf("pointer: %p\ntitle: %s\nquality: %f\n", (void*) cell.pointer, cell.title, cell.quality);

    // insert struct in cache
    memcpy(cell.pointer, &cell, sizeof(struct memory_cell));

    // insert image in cache
    memcpy(cell.pointer + sizeof(struct memory_cell), file, file_length);

    //printf("item insert with success\n");

    // return value
    return 1;

}

void cache_print(char *cache) {

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    struct memory_cell* cells = (struct memory_cell*) cache;

    //printf("*** PRINT CACHE ***\n");

    for(unsigned int i = 0; i < length && strcmp(cells -> title, "") != 0 ; ){

        //printf("title : %s\npointer: %p\nquality: %f\nlength: %lu\nnext: %lu\ntime: %lld\n",
                cells -> title, (void*) cells -> pointer, cells -> quality, cells -> length, cells -> next, (long long) cells -> seconds);

        i += (cells -> next) + sizeof(struct memory_cell);
        cache += i;
        cells = (struct memory_cell*) cache;

    }

}

// check if one item is in the cache
int cache_check(char *cache, struct memory_cell **cell, char *name, double quality_image,
                size_t imageLength, size_t imageWidth){

    //cache_print(cache);
    //printf("*** CACHE CHECK ****\n");

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));

    struct memory_cell* cells = (struct memory_cell*) cache;

    for(unsigned int i = 0; i < length  && strcmp(cells -> title, "") != 0; ){

        //printf("control checkMemory cell title: %s\n", cells -> title);
        //printf("control checkMemory title: %s\n", name);
        //printf("control checkMemory quality: %f\n", cells -> quality);
        //printf("control checkMemory cell quality: %f\n", quality_image);
        //printf("control checkMemory cache: %p\n", cells);
        //printf("control checkMemory i: %d\n", i);

        if(strcmp(cells -> title, name) == 0 && cells -> quality == quality_image &&
            cells -> image_length == imageLength && cells -> image_width == imageWidth){

            //sleep(1);

            cells -> seconds = time(NULL);
            *cell = cells;
            return(i);

        }

        i += (cells -> next) + sizeof(struct memory_cell);
        cache += i;
        cells = (struct memory_cell*) cache;

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

    //printf("*** CACHE INITIALIZE ***\n");

    **((unsigned int**) cache) = length - sizeof(unsigned int);

    *cache += sizeof(unsigned int);

    //printf("cache: %p\n", cache);

    return 1;

}