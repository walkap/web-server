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

double get_double(char *str);
double parse_weight(char *str);
char *parse_user_agent(char *str);
void parse_request_line(struct http_request *r, char *t);
struct http_request *parse_request(char *str);

#endif //WEB_SERVER_HTTP_PARSER_H
