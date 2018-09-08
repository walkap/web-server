#ifndef WEB_SERVER_LOGGING_H
#define WEB_SERVER_LOGGING_H

#include "../utils.h"
#include "../http_parsing/response_setter.h"
#include "../http_parsing/request_parser.h"

FILE *open_logfile(char *s);
void logging(struct http_request *pt, char *str, size_t lenght);

#endif //WEB_SERVER_LOGGING_H
