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

    /*write log*/
    logging(pt, response, lenght);

}

/**
 * This method searches if the image requested is present in the server
 * @param filename
 * @return
 */
int search_files(char*dir, char * filename){

    char *path = malloc(strlen(dir) + strlen(filename) + 1);
    exit_on_error(path == NULL, "error in malloc");
    strcpy(path, dir);
    strcat(path, filename);
    FILE *file;
    if ((file = fopen(path, "r"))){
        fclose(file);
        return 1;
    }
    return 0;
}

/**
 * This method reads a html file and saves it in a buffer
 * @param str2
 * @param len
 * @return
 */
char *read_file(char* dir, char *str2, size_t *len) {

    char *path = malloc(strlen(dir) + strlen(str2) + 1);
    exit_on_error(path == NULL, "error in malloc");
    strcpy(path, dir);
    strcat(path, str2);

    char *fbuffer;
    int length;
    FILE *f;

    if ((f = fopen(path, "r")) == NULL) {
        perror("error opening file");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    fbuffer = malloc(sizeof(char) * length);
    exit_on_error(fbuffer == NULL, "error in malloc");
    int total = 0;
    if (fbuffer) {
        while (total != length) {
            total += fread(fbuffer, 1, length, f);
        }
    }
    fclose(f);
    *len = length;
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

    char *buff = malloc(sizeof(char) * DIM_HEADER);
    exit_on_error(buff == NULL, "error in malloc");

    if (status == 200) {
        //TODO define macros for string like content-type
        snprintf(buff, DIM_HEADER,
                 "%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %lu\r\nConnection: keep-alive\r\n\r\n", version,
                 type,
                 len);
        fflush(stdout);

    } else if (status == 400) {
        snprintf(buff, DIM_HEADER,
                 "%s 400 Bad Request\r\nContent-Type: %s\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n", version,
                 type, len);

    } else if (status == 404) {

        snprintf(buff, DIM_HEADER,
                 "%s 404 Not Found\r\nConnection: close\r\n\r\n",
                 version);

    } else {
        return NULL;
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

    char *fbuffer = NULL;
    char *response;
    size_t hlen = 0;
    size_t lenght = 0;
    char *buff = malloc(BUFFDIM);
    exit_on_error(buff == NULL, "error in malloc");

    size_t *imgsize = malloc(sizeof(size_t));
    exit_on_error(imgsize == NULL, "error in malloc");

    if (req->invalid_request) {

        fbuffer = read_file(FILE_DIR, "/400.html", &lenght);

        response = build_header(400, "text/html", (int) lenght, req->version);
        exit_on_error(response == NULL, "error in build header");

        hlen = strlen(response);
        memcpy(buff, response, hlen);

    }
    else if (search_files(IMAGE_DIR, req->uri)) {

        double q = parse_weight(req->accept);
        char *u_a = parse_user_agent(req->user_agent);
        int rv;
        size_t width, height;

        const char **info = malloc(10 * sizeof(char));
        exit_on_error(info == NULL, "error in malloc");

        printf("\nimage quality: %.2f\n", q);
        fflush(stdout);

        printf("\nuser agent: %s\n", u_a);
        fflush(stdout);

        rv = get_info(u_a, info);
        if (rv == -1) {
            height = 1200;
            width = 1200;
        }
        else{
            char *pt;
            width = (size_t) strtol(info[0], &pt, 0);
            exit_on_error(*pt != '\0', "error in strtol width");
            height = (size_t) strtol(info[1], &pt, 0);
            exit_on_error(*pt != '\0', "error in strtol height");
        }

        struct memory_cell *cell = malloc(sizeof(struct memory_cell));
        exit_on_error(cell == NULL, "error in malloc");

        if (cache_check(CACHE, &cell, req->uri, q, height, width) != -1) {

            printf("CACHE HIT\n");
            fbuffer = malloc(cell->length);
            exit_on_error(fbuffer == NULL, "error in malloc");
            memcpy(fbuffer, cell->pointer, cell->length);
            response = build_header(200, "image/jpeg", cell->length, req->version);
            hlen = strlen(response);
            memcpy(buff, response, hlen);

            lenght = cell->length;

        } else {

            printf("IN Process\n");
            fbuffer = (char *) process_image(req->uri, width, q, imgsize);
            printf("DEBUG\n");

            response = build_header(200, "image/jpeg", *imgsize, req->version);
            hlen = strlen(response);
            memcpy(buff, response, hlen);

            printf("Inserting\n");

            rv = pthread_mutex_lock(&mutex);
            exit_on_error(rv != 0, "error in pthread_mutex_lock");

            cache_insert(CACHE, (void *) fbuffer, *imgsize, req->uri, q,
                         height, width);

            rv = pthread_mutex_unlock(&mutex);
            exit_on_error(rv != 0, "error in pthread_mutex_unlock");

            lenght = *imgsize;
            free(cell);

        }

    } else if (search_files(FILE_DIR, req->uri)) {

        char* type = NULL;
        fbuffer = read_file(FILE_DIR, req->uri, &lenght);

        if (strstr(req->uri, ".css") != NULL) {
            type = "text/css";
        }
        else if (strstr(req->uri, ".html") != NULL) {
            type = "text/html";
        }
        else if (strstr(req->uri, ".js") != NULL) {
            type = "application/javascript";
        }
        else if (strstr(req->uri, ".jpg") != NULL) {
            type = "image/jpeg";
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

    if (strcmp(req->method, "GET") == 0) {
        memcpy(buff + hlen, fbuffer, lenght);
    }

    printf("\n%s\n", response);
    write_response(buff, hlen + lenght, conn, req);

    free(buff);
    free(response);
    free(fbuffer);
    free(imgsize);
    free(req);
}

/**
 * This method is used to specify if the request has a valid sintax and then builds a response
 * @param str
 * @param conn
 * @return
 */
int set_response(char *str, int conn) {

    int alive= 0;
    struct http_request *request;

    request = parse_request(str);


    if (request->alive) {
        alive = 1;
    }
    request->invalid_request=0;

    if (strcmp(request->method, "GET") != 0 &&
        strcmp(request->method, "HEAD") != 0) {
        request->invalid_request = 1;
    }

    build_response(request, conn);

    return alive;
}
