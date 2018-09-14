#include "response_setter.h"

/**This method writes the http response in the socket
 *
 * @param response
 * @param lenght
 * @param conn
 * @param pt
 */
void write_response(char *response, size_t lenght, int conn, struct http_request *pt) {
    ssize_t b_written;

    b_written = writen(conn, response, (size_t) lenght);
    exit_on_error(b_written == -1, "error in write");

    //Logging
    logging(pt, response, lenght);
}

/**
 * This method searches if the image requested is present in the server
 * @param filename
 * @return
 */
int is_file_present(char *dir, char *filename) {
    FILE *file;
    char *path;

    path = malloc(strlen(dir) + strlen(filename) + 1);
    exit_on_error(path == NULL, "error in malloc");
    //Concatenate strings and get the right path
    sprintf(path, "%s%s", dir, filename);

    if ((file = fopen(path, "r"))) {
        fclose(file);
        return 1;
    }
    free(path);
    return 0;
}

/**
 * This method reads a html file and saves it in a buffer
 * @param str2
 * @param len
 * @return
 */
char *read_file(char *dir, char *str2, size_t *len) {
    char *fbuffer, *path;
    size_t total;
    size_t length;
    FILE *file;

    //TODO Missing path free
    //Allocate enough memory for the file path
    path = malloc(strlen(dir) + strlen(str2) + 1);
    exit_on_error(path == NULL, "error in malloc");
    //Concatenate strings and build the right path to the file
    sprintf(path, "%s%s", dir, str2);
    //Open the file at the specified path
    if ((file = fopen(path, "r")) == NULL) {
        perror("error opening file");
        exit(1);
    }
    //Position the cursor to the end of the file
    fseek(file, 0, SEEK_END);
    //Get the file size in byte
    length = (size_t) ftell(file);
    //Reposition the cursor
    fseek(file, 0, SEEK_SET);
    //Allocate enough memory to store the file opened
    fbuffer = malloc(sizeof(char) * length);
    exit_on_error(fbuffer == NULL, "error in malloc");
    //Get the file size
    total = 0;
    if (fbuffer) {
        while (total != length) {
            total += fread(fbuffer, 1, length, file);
        }
    }
    *len = length;
    //Close the file then return the buffer
    fclose(file);
    return fbuffer;
}

/**
 * This method builds the header of the http response header according to parameters
 * @param status
 * @param type
 * @param len
 * @param version
 * @return
 */
char *build_header(int status, char *type, size_t len, char *version) {
    char *buff;

    buff = malloc(sizeof(char) * DIM_HEADER);
    exit_on_error(buff == NULL, "error in malloc");

    switch (status){
        case 200:
            snprintf(buff, DIM_HEADER,
                     "%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %lu\r\nConnection: keep-alive\r\n\r\n", version,
                     type,
                     len);
            break;
        case 400:
            snprintf(buff, DIM_HEADER,
                     "%s 400 Bad Request\r\nContent-Type: %s\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n", version,
                     type, len);
            break;
        case 404:
            snprintf(buff, DIM_HEADER,
                     "%s 404 Not Found\r\nConnection: close\r\n\r\n",
                     version);
            break;
        default:
            break;

    }
    return buff;
}

/**
 * This method builds an http response message according to the uri requested. If the uri is an image,
 * the cache is checked, if the image is not in the cache, it is processed, saved in cache and sent in the response.
 * @param req
 * @param conn
 */
void build_response(struct http_request *req, int conn) {
    char *fbuffer = NULL, *response;
    char *buff = NULL;
    size_t *imgsize;
    size_t hlen = 0, lenght = 0;

    struct memory_cell *cell;
    int info[2];
    double q;
    char *u_a;
    size_t width, height;

    //TODO this should be dynamic
    //Allocate memory for the content to sent over the socket
    buff = malloc(BUFFDIM);
    exit_on_error(buff == NULL, "error in malloc");

    //Allocate memory to store the image size pointer
    imgsize = malloc(sizeof(size_t));
    exit_on_error(imgsize == NULL, "error in malloc");

    if (req->invalid_request) {
        //Read the file and get the file lenght
        fbuffer = read_file(FILE_DIR, "/400.html", &lenght);
        //Build header response
        response = build_header(400, "text/html", (int) lenght, req->version);
        exit_on_error(response == NULL, "error in build header");
        hlen = strlen(response);
        //Copy the header response in the buffer
        memcpy(buff, response, hlen);
    } else if (is_file_present(IMAGE_DIR, req->uri)) { //Search an image
        //TODO this is not good we should check in the cache before then on the file system
        //TODO if the image size is not present then call image_process if the original image exists
        if (req->accept == NULL) {
            q = 1.0;
        } else {
            q = parse_weight(req->accept);
        }
        //Get the user agent from browser
        u_a = parse_user_agent(req->user_agent);
        //Allocate memory to store the images sizes
        /* info = malloc(10 * sizeof(int));
         exit_on_error(info == NULL, "error in malloc");*/
        printf("\nImage quality: %.2f\n", q);
        fflush(stdout);
        printf("\nUser agent: %s\n", u_a);
        fflush(stdout);
        //TODO this conditional should be deleted because is the ua in unknown the size should be the original one
        //TODO SEE set_with function that does the job
        //Get image sizes from the user agent
        get_info(u_a, info, req->uri);
        width = (size_t) info[0];
        height = (size_t) info[1];
        /*char *pt;
            width = (size_t) strtol(info[0], &pt, 0);
            exit_on_error(*pt != '\0', "error in strtol width");
            height = (size_t) strtol(info[1], &pt, 0);
            exit_on_error(*pt != '\0', "error in strtol height");*/
        //Allocate memory for cache struct
        cell = malloc(sizeof(struct memory_cell));
        exit_on_error(cell == NULL, "error in malloc");
        //Check whether an image is in the cache or not
        if (cache_check(CACHE, &cell, req->uri, q, height, width) != -1) {
            printf("CACHE HIT\n");
            response = build_header(200, "image/png", cell->length, req->version);
            hlen = strlen(response);
            memcpy(buff, response, hlen);
            fbuffer = cell->pointer + sizeof(struct memory_cell);
            lenght = cell->length;
        } else {
            printf("In process\n");
            //TODO here should placed a condition to check on file system if the image exists before process it
            //Process an image with the new width and quality
            fbuffer = (char *) process_image(req->uri, width, (float_t) q, imgsize);
            //Once get the image from the script create a response
            response = build_header(200, "image/jpeg", *imgsize, req->version);
            //Copy the response into the buffer
            hlen = strlen(response);
            memcpy(buff, response, hlen);
            //Get the mutex lock
            exit_on_error(pthread_mutex_lock(&mutex) != 0, "error in pthread_mutex_lock");
            //Insert item in the cache
            cache_insert(CACHE, (void *) fbuffer, *imgsize, req->uri, q, height, width);
            //Unlock the mutex
            exit_on_error(pthread_mutex_unlock(&mutex) != 0, "error in pthread_mutex_unlock");
            //Save the image length
            lenght = *imgsize;
        }
    } else if (is_file_present(FILE_DIR, req->uri)) {
        //TODO why search a file and just after check if is an image or file? We should do this before
        //TODO also if is an image we cannot find it in the FILE_DIR
        //TODO so we should first check what kind of file then process
        //TODO and we don't need FILE_DIR and IMAGE_DIR because in the request there is also the folder so we just need the root folder
        char *type = NULL;
        fbuffer = read_file(FILE_DIR, req->uri, &lenght);
        if (strstr(req->uri, ".css") != NULL) {
            type = "text/css";
        } else if (strstr(req->uri, ".html") != NULL) {
            type = "text/html";
        } else if (strstr(req->uri, ".js") != NULL) {
            type = "application/javascript";
        }
        response = build_header(200, type, lenght, req->version);
        exit_on_error(response == NULL, "error in build header");
        hlen = strlen(response);
        memcpy(buff, response, hlen);
    } else {
        response = build_header(404, " ", 0, req->version);
        hlen = strlen(response);
        memcpy(buff, response, hlen);
    }
    //Check the method //TODO what if is false?
    if (strcmp(req->method, "GET") == 0) {
        memcpy(buff + hlen, fbuffer, lenght);
    }
    printf("%s\n", response);
    write_response(buff, hlen + lenght, conn, req);
    free(response);
    free(buff);
    free(req);
}


/**
 * This method is used to specify if the request has a valid sintax and then builds a response
 * @param str
 * @param conn
 * @return
 */
int set_response(char *str, int conn) {
    int alive = 0;
    struct http_request *request;
    request = parse_request(str);
    if (request->alive) {
        alive = 1;
    }
    request->invalid_request = 0;
    if (strcmp(request->method, "GET") != 0 &&
        strcmp(request->method, "HEAD") != 0) {
        request->invalid_request = 1;
    }
    build_response(request, conn);
    return alive;
}