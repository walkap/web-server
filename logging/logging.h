#ifndef WEB_SERVER_LOGGING_H
#define WEB_SERVER_LOGGING_H

#include "../utils.h"
#include "../http_parsing/response_setter.h"
#include "../http_parsing/request_parser.h"

#define LOG_FILE "../logfile"

FILE *open_logfile(char *s);
void logging(struct http_request *pt, char *str);

#endif //WEB_SERVER_LOGGING_H