#ifndef WEB_SERVER_HTTP_PARSER_H
#define WEB_SERVER_HTTP_PARSER_H

#include "../utils.h"

struct http_request {

    char *method;
    char *uri;
    char *version;
    char *accept;
    char *user_agent;
    int alive;
};

double get_double(char *str)
{
    while (*str && !(isdigit(*str) || ((*str == '-' || *str == '+') && isdigit(*(str + 1)))))
        str++;

    return strtod(str, NULL);
}

double parse_weight(char *str)
{
    char* s = "q";
    char *token;

    token=strtok(str, s);
    token=strtok(NULL, s);

    double q = get_double(token);

    return q;

}
char *parse_user_agent(char *str)
{
    char* s = " ";
    char* token;

    token=strtok(str, s);
    token=strtok(NULL, s);


    return token;

}

void parse_request_line(struct http_request *r, char* t)
{
    char *sc = " ";
    char *token;

    token = strtok(t, sc);
    exit_on_error(t == NULL, "error in strtok in parse_request_line");

    if (strstr(t, "GET")!= NULL)
        r->method = "GET";
    if (strstr(token, "HEAD")!= NULL)
        r->method = "HEAD";

    token = strtok(NULL, sc);
    exit_on_error(token == NULL, "error in strtok in parse_request_line");

    r->uri=token;

    sc="\r";

    token = strtok(NULL, sc);
    exit_on_error(token == NULL, "error in strtok in parse_request_line");

    r->version= token;

}


struct http_request* parse_request(char *str)
{
    struct http_request* request;
    const char s[2] = "\n";
    char *token;

    request = malloc(sizeof(struct http_request));
    exit_on_error(request == NULL, "error in malloc");

    request->alive = 0;
    int len = strlen(str);
    char *buff = malloc(len + 1);
    exit_on_error(buff == NULL, "error in malloc");
    strcpy(buff, str);

        token = strtok(buff, s);
        exit_on_error(token == NULL, "error in strtok in parse_request");

        parse_request_line(request, buff);

    token = strtok(str, s);

    while( token != NULL ) {

        if (strstr(token, "User-Agent:")!= NULL) {
            request->user_agent=token;
        }
        if (strstr(token, "Accept:")!= NULL) {
            request->accept=token;

        }
        if (strstr(token, "Connection: keep-alive")!= NULL) {
            request->alive=1;
        }

        token = strtok(NULL, s);
    }

    return request;

}

#endif //WEB_SERVER_HTTP_PARSER_H
