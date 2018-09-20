#include "cache.h"

 /**
  * Find the first empty byte and return the numbers of bytes
  * from cache start (-1 for error)
  * @param cache
  * @param cell
  * @return int
  */
int find_empty_cell(char *cache, struct memory_cell **cell){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));
    int sum = 0;
    struct memory_cell* cells = (struct memory_cell*) cache;

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memory_cell)){
        if(strcmp(cells -> title, "") == 0){
            *cell = cells;
            return(sum);
        }
        cache += i;
        cells = (struct memory_cell*) cache;
        sum += i;
    }
    return -1;
}

/**
 * Find the less useful cache item and return the numbers of bytes from cache start (-1 for error)
 *
 * @param cache
 * @param file_length
 * @param cell
 * @return int
 */
int find_less_useful(char *cache, size_t file_length, struct memory_cell **cell){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));
    int sum = 0;
    int position_less_useful = 0;
    struct memory_cell* cells = (struct memory_cell*) cache;
    struct memory_cell* less_useful = cells;
    if((cells -> next) < file_length){
        position_less_useful = -1;
    }

    for(unsigned int i = 0; i < length ; i += (cells -> next) + sizeof(struct memory_cell)){
        cache += i;
        cells = (struct memory_cell*) cache;
        sum += i;

        if(strcmp(cells -> title, "") == 0 && (cells -> next) >= file_length &&
           cells -> seconds < less_useful -> seconds){
            less_useful = cells;
            position_less_useful = sum;
        }

    }
    if(position_less_useful != -1){
        *cell = less_useful;
    }
    return position_less_useful;
}

/**
 * Insert one item in cache and return 1 for success (-1 for error)
 * @param cache
 * @param file
 * @param file_length
 * @param name
 * @param quality_image
 * @return int
 */
int cache_insert(char *cache, void *file, size_t file_length, char *name, double quality_image){

    // generate cellMem
    struct memory_cell cell;
    struct memory_cell* empty_cell;

    strcpy(cell.title, name);
    cell.length = file_length;
    cell.next = file_length;
    cell.quality = quality_image;
    cell.seconds = time(NULL);

    // check for empty cell (change)
    int bytes_number = find_empty_cell(cache, &empty_cell);

    // find the cell to eliminate if the cell are full
    if(bytes_number == -1){
        // find the less useful cache cell
        bytes_number = find_less_useful(cache, file_length, &empty_cell);

        if(bytes_number == -1){
            fprintf(stderr, "cache full and not useful find\n");
            return -1;
        }
        cell.next = ((struct memory_cell*) (cache + bytes_number)) -> next;
    }

    // insert pointer in cell
    cell.pointer = cache + bytes_number;

    // insert struct in cache
    memcpy(cell.pointer, &cell, sizeof(struct memory_cell));

    // insert image in cache
    memcpy(cell.pointer + sizeof(struct memory_cell), file, file_length);

    return 1;
}

/**
 * check if one item is in the cache
 * @param cache
 * @param cell
 * @param name
 * @param quality_image
 * @return int
 */
int cache_check(char *cache, struct memory_cell **cell, char *name, double quality_image){

    unsigned int length = *((unsigned int*) (cache - sizeof(unsigned int)));
    struct memory_cell* cells = (struct memory_cell*) cache;
    for(unsigned int i = 0; i < length  && strcmp(cells -> title, "") != 0; ){
        if(strcmp(cells -> title, name) == 0 && cells -> quality == quality_image){
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

/**
 * Inizialize the cache
 * @param numb
 * @param cache
 * @return int
 */
int cache_initialize(size_t numb, char **cache){

    unsigned int length = (unsigned int)numb *  getpagesize() - sizeof(unsigned int);
    // create cache pointer and calloc
    *cache = (char*) calloc(1, length);
    if(*cache == NULL){
        return -1;
    }
    **((unsigned int**) cache) = length - sizeof(unsigned int);
    *cache += sizeof(unsigned int);

    return 1;
}