#ifndef WEB_SERVER_RESPONSE_H
#define WEB_SERVER_RESPONSE_H

#include "request_parser.h"
#include "../utils.h"
#include "../logging/logging.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>

//Definitions
void write_response(char *response, int len, int conn, struct http_request *pt);
char *read_image(char *str2, int *len);
char *build_header(int status, char *type, int len, char *version);
off_t get_file_size(int fd);
void html_content(const char *dest, char *fptr);
void build_response(struct http_request *req, int conn);
int set_response(char *str, int conn);

#endif //WEB_SERVER_RESPONSE_H