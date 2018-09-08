#include "response_setter.h"

//TODO we should set the html root somehow so we can just use something like INDEX "index.html" insted the complete relative path
//#define HTML_INDEX ".../www/index.html"
//#define HTML_400 ".../www/400.html"

void write_response(char *response, size_t lenght, int conn, struct http_request *pt) {

    ssize_t b_written;
    printf("LEN  %lu\n", lenght);

    printf("\n%s\n", response);
    fflush(stdout);

    b_written = write(conn, response, (size_t) lenght);
    exit_on_error(b_written == -1, "error in write");

    logging(pt, response, lenght);
}

char *read_image(char *str2, int *len) {
    char *path = malloc(strlen(IMAGE_DIR) + strlen(str2) + 1);
    exit_on_error(path == NULL, "error in malloc");
    strcpy(path, IMAGE_DIR);
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
 * Copies the content of a file in a char *
 * @param destination buffer where copy the file content
 * @param fptr file path
 */
void html_content(const char *dest, char *fptr) {

    int fd;
    char *html;

    //Open file descriptor
    fd = open(fptr, O_RDONLY);
    if (fd == -1) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    //Get the file size
    off_t size = get_file_size(fd);

    //Allocate dynamic memory with the size file
    html = malloc(sizeof(char) * size);
    if (html == NULL) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    //Read the file and save the content into the buffer
    if (read(fd, html, (size_t) size) == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }

    //Close the file descriptor
    close(fd);

    //Assign the value to the destination
    dest = html;

    //Free memory
    free(html);
}

void build_response(struct http_request *req, int conn) {

    char *fbuffer = NULL;
    char *response;
    size_t hlen = 0;
    size_t lenght = 0;
    char buff[500048];

    size_t *imgsize = malloc(sizeof(size_t));
    exit_on_error(imgsize == NULL, "error in malloc");

    //TODO file checking should be general
    if (strcmp(req->method, "invalid") == 0) {

        fbuffer = HTML_400;
        lenght = strlen(fbuffer);

        response = build_header(400, "text/html", (int) lenght, req->version);
        exit_on_error(response == NULL, "error in build header");

        hlen = strlen(response);
        memcpy(buff, response, hlen);

    } else if (strcmp(req->uri, "/index.html") == 0) {

        fbuffer = HTML_INDEX;
        lenght = strlen(fbuffer);

        response = build_header(200, "text/html", lenght, req->version);
        exit_on_error(response == NULL, "error in build header");

        hlen = strlen(response);
        memcpy(buff, response, hlen);

    } else if (strcmp(req->uri, "/wizard.jpg") == 0) {

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
        } else {
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
            char *img = malloc(cell->length);
            exit_on_error(img == NULL, "error in malloc");
            memcpy(img, cell->pointer, cell->length);
            response = build_header(200, "image/gif", cell->length, req->version);
            hlen = strlen(response);
            memcpy(buff, response, hlen);

            lenght = cell->length;
            fbuffer = img;

        } else {

            printf("IN Process\n");
            fbuffer = (char *) process_image(req->uri, width, height, q, imgsize);
            printf("DEBUG\n");

            response = build_header(200, "image/gif", *imgsize, req->version);
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

        }

    } else {
        response = build_header(404, " ", 0, req->version);
        hlen = strlen(response);
        memcpy(buff, response, hlen);
    }

    if (strcmp(req->method, "GET") == 0) {
        memcpy(buff + hlen, fbuffer, lenght);
    }

    write_response(buff, hlen + lenght, conn, req);

}

int set_response(char *str, int conn) {
    int alive;
    struct http_request *request;

    request = parse_request(str);

    if (request->alive) {
        alive = 1;
    } else {
        alive = 0;
    }

    if (strcmp(request->method, "GET") != 0 &&
        strcmp(request->method, "HEAD") != 0) {
        request->method = "invalid";
    }

    build_response(request, conn);

    return alive;
}
