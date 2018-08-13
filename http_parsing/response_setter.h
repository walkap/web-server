#ifndef WEB_SERVER_RESPONSE_H
#define WEB_SERVER_RESPONSE_H

#include "request_parser.h"
#include "../utils.h"
#include "../logging/logging.h"


void write_response(char *response, int len, int conn, struct http_request* pt)
{

    int b_written;
    printf("%s\n", response);
    b_written = write(conn, response, len);
    exit_on_error(b_written == -1, "error in write");

    logging(pt, response, len);

}

char *read_image(char *str2, int *len)
{
    char* str1="../storage";
    char *path = malloc(strlen(str1) + strlen(str2) + 1);
    exit_on_error(path == NULL, "error in malloc");
    strcpy(path, str1);
    strcat(path, str2);

    printf("%s\n", path);
    char *fbuffer;
    int length;
    FILE *f;

    if ((f = fopen(path, "r")) == NULL)
    {
        perror("error opening file");
        exit(1);
    }

    fseek (f, 0, SEEK_END);
    length = ftell(f);
    fseek (f, 0, SEEK_SET);
    fbuffer = malloc(sizeof(char)*length);
    exit_on_error(fbuffer == NULL, "error in malloc");
    int total = 0;
    if (fbuffer)
    {
        while (total != length)
        {
            total += fread(fbuffer, 1, length, f);
        }
    }
    fclose (f);
    *len = length;
    return fbuffer;
}

char *build_header(int status, char *type, int len, char* version)
{
    char* buff = malloc(sizeof(char) * DIM_HEADER);
    exit_on_error(buff == NULL, "error in malloc");


    if (status == 200) {

        snprintf(buff, DIM_HEADER,
                        "%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: keep-alive\r\n\r\n", version, type, len);
        printf("buffer : %s\n", buff);
        fflush(stdout);

    }
    else if (status == 400) {

        snprintf(buff, DIM_HEADER,
                        "%s 400 Bad Request\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: close\r\n\r\n", version, type,len);

    }
    else if (status == 404) {

        snprintf(buff, DIM_HEADER,
                        "%s 404 Not Found\r\nConnection: close\r\n\r\n",
                        version);

    }
    else{
        return NULL;
    }

    return buff;
}

void build_response(struct http_request *req, int conn)
{
    char *fbuffer;
    char *response;
    int hlen;
    int lenght = 0;
    char buff[50048];


    if (strcmp(req -> method, "invalid")==0) {

        fbuffer = HTML_400;
        lenght = strlen(HTML_400);

        response = build_header(400, "text/html", lenght, req->version);
        exit_on_error(response == NULL, "error in build header");

        hlen = strlen(response);
        memcpy(buff, response, hlen);


    }
    else if (strcmp(req -> uri, "/index.html")==0) {

        fbuffer = HTML_INDEX;
        lenght = strlen(HTML_INDEX);

        response = build_header(200, "text/html", lenght, req->version);
        exit_on_error(response == NULL, "error in build header");

        hlen = strlen(response);
        memcpy(buff, response, hlen);


    }
    else if (strcmp(req -> uri, "/wizard.jpg")==0) {

        //TODO check if image is present in cache

        fbuffer = read_image(req->uri, &lenght);

        double q = parse_weight(req->accept);

        printf("%f  %s\n", q, req->user_agent);

        //TODO modify image given weight q and User-Agent header line

        response = build_header(200, "image/gif", lenght, req->version);


        hlen = strlen(response);


        memcpy(buff, response, hlen);

    }

    else{

        response = build_header(404, " ", 0, req->version);
        hlen = strlen(response);
        memcpy(buff, response, hlen);

    }


    if (strcmp(req->method, "GET")==0) {
        memcpy(buff + hlen, fbuffer, lenght);
    }

    write_response(buff, hlen+lenght, conn, req);

}

int set_response(char *str, int conn)
{
    int alive;
      struct http_request* request;

      request = parse_request(str);

      if (request->alive) {
          alive = 1;
      }
      else{
          alive = 0;
      }

    if (strcmp(request -> method, "GET")!=0 &&
        strcmp(request -> method, "HEAD")!=0) {
        request->method="invalid";
        }

    build_response(request, conn);

      return alive;
}

#endif //WEB_SERVER_RESPONSE_H
