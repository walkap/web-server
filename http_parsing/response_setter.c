#include "response_setter.h"
#include "../cache/cache.h"
#include "../ua_parsing/ua_parsing.h"
#include "../image_processing/image_processing.h"

char CACHE[200000];

//TODO we should set the html root somehow so we can just use something like INDEX "index.html" insted the complete relative path
//#define HTML_INDEX ".../www/index.html"
//#define HTML_400 ".../www/400.html"

void write_response(char *response, int len, int conn, struct http_request *pt) {

    int b_written;

    printf("\n%s\n", response);
    fflush(stdout);

    b_written = write(conn, response, len);
    exit_on_error(b_written == -1, "error in write");

    logging(pt, response, len);

}

char *read_image(char *str2, int *len) {
    char *str1 = "../storage";
    char *path = malloc(strlen(str1) + strlen(str2) + 1);
    exit_on_error(path == NULL, "error in malloc");
    strcpy(path, str1);
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

char *build_header(int status, char *type, int len, char *version) {
    char *buff = malloc(sizeof(char) * DIM_HEADER);
    exit_on_error(buff == NULL, "error in malloc");


    if (status == 200) {

        snprintf(buff, DIM_HEADER,
                 "%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: keep-alive\r\n\r\n", version, type,
                 len);
        fflush(stdout);

    } else if (status == 400) {

        snprintf(buff, DIM_HEADER,
                 "%s 400 Bad Request\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: close\r\n\r\n", version,
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
 * Returns the size of the content of the file
 * @param fd file descriptor
 * @return  off_t the offset of the file
 */
//TODO this could be shared with the whole project
off_t get_file_size(int fd) {
    int stat;
    struct stat info;

    //Get file info and save into the struct created
    stat = fstat(fd, &info);
    if (stat == -1) {
        perror("fstat()");
        exit(EXIT_FAILURE);
    }
    //Get the result
    off_t size = info.st_size;

    return size;
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
    char *fbuffer =  NULL;
    char *response;
    int hlen;
    int lenght = 0;
    char buff[50048];

    //TODO too much vars without cast or wrong type
    if (strcmp(req->method, "invalid") == 0) {

        fbuffer = HTML_400;
        lenght = strlen(fbuffer);

        response = build_header(400, "text/html", lenght, req->version);
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

        char *p = CACHE;
        double q = parse_weight(req->accept);
        printf("\nimage quality: %.2f\n", q);
        fflush(stdout);
        char *u_a = parse_user_agent(req->user_agent);
        printf("\nuser agent: %s\n", u_a);
        fflush(stdout);
        const char **info = get_info(u_a);
        int width = (int) info[0];
        int height = (int) info[1];

        printf("image heightxwidth %i %i", width, height);
        struct memory_cell *cell = malloc(sizeof(struct memory_cell));
        exit_on_error(cell == NULL, "error in malloc");

        if (check_memory(p, 200000, &cell, req->uri, q, height, width) != -1) {

            char *img = malloc(50000);
            exit_on_error(img == NULL, "error in malloc");

            memcpy(img, cell->pointer + sizeof(struct memory_cell), cell->length);

            fbuffer = img;

        } else {

            char *img = read_image(req->uri, &lenght);

            printf("IN Process\n");

            fbuffer = process_image(img, width, height, q);

            response = build_header(200, "image/gif", lenght, req->version);

            hlen = strlen(response);

            memcpy(buff, response, hlen);

            printf("Inserting\n");

            cache_insert(p, 200000, (FILE *) fbuffer, req->uri, q,
                         height, width);
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
