#ifndef WEB_SERVER_HTTP_PARSER_H
#define WEB_SERVER_HTTP_PARSER_H

#include "../utils.h"

struct http_request {
    char *method;
    char *uri;
    char *version;
    char *accept;
    char *user_agent;
    int invalid_request;
    int alive;
};

double parse_weight(char *str);
char *parse_user_agent(char *str);
void parse_request(char *str, struct http_request *request);

#endif //WEB_SERVER_HTTP_PARSER_H
