#ifndef WEB_SERVER_HTTP_PARSER_H
#define WEB_SERVER_HTTP_PARSER_H

#include "../utils.h"

struct http_request {

    char *method;
    char *uri;
    char *accept;
    char *user_agent;
};

void parse_request_line(struct http_request *r, char* t)
{
    char *s = " ";
    char *token;

    token = strtok(t, s);
    exit_on_error(t == NULL, "error in strtok in parser_request_line");

    if (strstr(t, "GET")!= NULL)
        r->method = "GET";
    if (strstr(token, "HEAD")!= NULL)
        r->method = "HEAD";

    token = strtok(NULL, s);
    exit_on_error(token == NULL, "error in strtok in parser_request_line");

    r->uri=token;



}

void parse_header_line(struct http_request *r, char* t, char* type)
{
    char *s = " ";
    char *token;

    token = strtok(t, s);
    exit_on_error(token == NULL, "error in strtok in parser_header_line");

    token = strtok(NULL, s);
    exit_on_error(token == NULL, "error in strtok in parser_header_line");

    if (strcmp(type, "user_agent")==0)
        r->user_agent=token;
    if (strcmp(type, "accept")==0)
        r->accept=token;


}
struct http_request* parse_request(char *str)
{
    struct http_request* request;
    char *s = "\n";
    char *token;
    request = malloc(sizeof(struct http_request));
    exit_on_error(request == NULL, "error in malloc");

    token = strtok(str, s);
    exit_on_error(token == NULL, "error in strtok in parse_request");

    parse_request_line(request, token);

    while( token != NULL ) {

        if (strstr(token, "User-Agent:")!= NULL)
            parse_header_line(request, token, "user_agent");
        else if (strstr(token, "Accept:")!= NULL)
            parse_header_line(request, token,"accept");


        token = strtok(NULL, s);
    }

    return request;

}

#endif //WEB_SERVER_HTTP_PARSER_H
