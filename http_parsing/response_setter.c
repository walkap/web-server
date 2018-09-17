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
    logging(pt, response);
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
    //Allocate enough memory for the file path
    path = malloc(strlen(dir) + strlen(str2) + 1);
    exit_on_error(path == NULL, "error in malloc");
    //Concatenate strings and build the right path to the file
    sprintf(path, "%s%s", dir, str2);
    //Open the file at the specified path
    file = fopen(path, "r");
    exit_on_error(file == NULL, "error in fopen");
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

    switch (status) {
        case 200:
            snprintf(buff, DIM_HEADER,
                     "%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %lu\r\nConnection: keep-alive\r\n\r\n", version,
                     type,
                     len);
            break;
        case 400:
            snprintf(buff, DIM_HEADER,
                     "%s 400 Bad Request\r\nContent-Type: %s\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n",
                     version,
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
 * This function return the original name from a requested image
 * @param requested_name
 * @return
 */
char * get_original_image_name(char *requested_name) {
    char *src = malloc(strlen(requested_name)+1);
    exit_on_error(src == NULL, "error in malloc");
    strcpy(src, requested_name);
    char *token, *new_name, *delimiter;
    delimiter = "-";
    token = strtok(src, delimiter);
    exit_on_error(token == NULL, "error in token get_original_image_name");
    new_name = malloc(strlen(token) + 5);
    sprintf(new_name, "%s%s", token, ".jpg");
    printf("%s\n", new_name);
    free(src);
    return new_name;
}

/**
 * This function parse the image name from request returnig the width of the image in order
 * to respect the structure name-widthw.jpg
 * @param str the image name
 * @return integer
 */
int parse_width(char *str) {
    char *src = malloc(strlen(str)+1);
    exit_on_error(src==NULL, "error in malloc");
    strcpy(src, str);
    char *token1, *token2, *pt;
    int output;
    strtok(src, "-");
    token1 = strtok(NULL, "-");
    exit_on_error(token1 == NULL, "error in token parse_width");
    token2 = strtok(token1, "w");
    exit_on_error(token2 == NULL, "error in token parse_width");
    output = (int) strtol(token2, &pt, 0);
    exit_on_error(*pt != '\0', "error in strtol width");
    free(src);
    return output;
}

/**
 * This method builds an http response message according to the uri requested. If the uri is an image,
 * the cache is checked, if the image is not in the cache, it is processed, saved in cache and sent in the response.
 * @param req
 * @param conn
 */
void build_response(struct http_request *req, int conn) {

    char *u_a, *header_response = NULL, *body_buffer = NULL, *full_buffer = NULL, *type = NULL, *original_image_name;
    struct memory_cell *cell;
    int info[2];
    double q;
    size_t header_lenght = 0, body_lenght = 0, width;
    size_t *imgsize;

    //Allocate memory for the content to sent over the socket
    full_buffer = malloc(BUFFDIM);
    exit_on_error(full_buffer == NULL, "error in malloc");

    //Allocate memory to store the image size pointer
    imgsize = malloc(sizeof(size_t));
    exit_on_error(imgsize == NULL, "error in malloc");

    if (req->invalid_request) {
        //Read the file and get the file lenght
        body_buffer = read_file(FILE_DIR, PAGE_400, &body_lenght);
        //Build header header_response
        header_response = build_header(400, TYPE_HTML, body_lenght, req->version);
        exit_on_error(header_response == NULL, "error in build header");
        header_lenght = strlen(header_response);
        //Copy the header header_response in the buffer
        memcpy(full_buffer, header_response, header_lenght);

    } else if (strstr(req->uri, ".jpg") != NULL) { //Check if the request is an image
        if (req->accept == NULL) {
            q = 1.0;
        } else {
            q = parse_weight(req->accept);
        }
        //Get the user agent from browser
        u_a = parse_user_agent(req->user_agent);
        //Get image sizes from the user agent
        if(get_screen_size_ua(u_a, info) == 0){
            width = (size_t ) parse_width(req->uri);
        }else{
            width = (size_t) info[0];
        }
        //Allocate memory for cache struct
        cell = malloc(sizeof(struct memory_cell));
        exit_on_error(cell == NULL, "error in malloc");
        //Check whether an image is in the cache or not
        if ((cache_check(CACHE, &cell, req->uri, q) != -1)) {
            printf("CACHE HIT\n");
            body_buffer = cell->pointer + sizeof(struct memory_cell);
            body_lenght = cell->length;
            header_response = build_header(200, TYPE_JPEG, cell->length, req->version);
            header_lenght = strlen(header_response);
            memcpy(full_buffer, header_response, header_lenght);
        } else {
            puts("CACHE MISS\n");
            //Check if the image is on the file system
            if (is_file_present(IMAGE_DIR, req->uri)) {
                //Get the image
                body_buffer = read_file(IMAGE_DIR, req->uri, &body_lenght);
                header_response = build_header(200, TYPE_JPEG, body_lenght, req->version);
                header_lenght = strlen(header_response);
                memcpy(full_buffer, header_response, header_lenght);

                exit_on_error(pthread_mutex_lock(&mutex) != 0, "error in pthread_mutex_lock");
                //Insert item in the cache
                cache_insert(CACHE, (void *) body_buffer, body_lenght, req->uri, q);
                //Unlock the mutex
                exit_on_error(pthread_mutex_unlock(&mutex) != 0, "error in pthread_mutex_unlock");
            } else {
                //Check if the original exists
                original_image_name = get_original_image_name(req->uri);
                if (is_file_present(IMAGE_DIR, original_image_name)) {
                    printf("In process\n");
                    //Process an image with the new width and quality
                    body_buffer = (char *) process_image(original_image_name, width, (float_t) q, imgsize);
                    //Once get the image from the script create a response
                    header_response = build_header(200, TYPE_JPEG, *imgsize, req->version);
                    //Copy the response into the buffer
                    header_lenght = strlen(header_response);
                    memcpy(full_buffer, header_response, header_lenght);
                    //Get the mutex lock
                    exit_on_error(pthread_mutex_lock(&mutex) != 0, "error in pthread_mutex_lock");
                    //Insert item in the cache
                    cache_insert(CACHE, (void *) body_buffer, *imgsize, req->uri, q);
                    //Unlock the mutex
                    exit_on_error(pthread_mutex_unlock(&mutex) != 0, "error in pthread_mutex_unlock");
                    //Save the image length
                    body_lenght = *imgsize;
                }
                free(original_image_name);
            }
        }
    } else {
        //Check whether a file is present or not
        if (is_file_present(FILE_DIR, req->uri)) {
            //Check the kind of file requested
            if (strstr(req->uri, ".css") != NULL) {
                type = TYPE_CSS;
            } else if (strstr(req->uri, ".html") != NULL) {
                type = TYPE_HTML;
            } else if (strstr(req->uri, ".js") != NULL) {
                type = TYPE_JS;
            }
            //Read the file will be our body
            body_buffer = read_file(FILE_DIR, req->uri, &body_lenght);
            //Build the header header_response
            header_response = build_header(200, type, body_lenght, req->version);
            exit_on_error(header_response == NULL, "error in build header");
            //Free the memory allocated for the type char pointer
            header_lenght = strlen(header_response);
            //Copy the header_response result in the buffer
            memcpy(full_buffer, header_response, header_lenght);
        } else {
            //Read the file and get the file lenght
            body_buffer = read_file(FILE_DIR, PAGE_404, &body_lenght);
            header_response = build_header(404, TYPE_HTML, body_lenght, req->version);
            header_lenght = strlen(header_response);
            memcpy(full_buffer, header_response, header_lenght);
        }
    }
    //Check the method if is not GET it is HEAD and we do not attach the body in the request
    if (strcmp(req->method, "GET") == 0) {
        memcpy(full_buffer + header_lenght, body_buffer, body_lenght);
    }

    printf("Print out the header_response\n%s\n", header_response);
    write_response(full_buffer, header_lenght + body_lenght, conn, req);

    free(header_response);
    free(full_buffer);
    free(imgsize);

}

/**
 * This method is used to specify if the request has a valid sintax and then builds a response
 * @param str - buffer used to store the request
 * @param conn -
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