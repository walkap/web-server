#ifndef WEB_SERVER_RESPONSE_H
#define WEB_SERVER_RESPONSE_H

#include "request_parser.h"
#include "../utils.h"
#include "../logging/logging.h"
#include "../cache/cache.h"
#include "../ua_parsing/ua_parsing.h"
#include "../image_processing/image_processing.h"

#define TYPE_JPEG "image/jpeg"
#define TYPE_HTML "text/html"
#define TYPE_CSS "text/css"
#define TYPE_JS "application/javascript"
#define PAGE_400 "/400.html"
#define PAGE_404 "/404.html"

//Definitions
void write_response(char *response, size_t lenght, int conn, struct http_request *pt);
char *build_header(int status, char *type, size_t len, char *version);
char *read_file(char* dir, char *str2, size_t *len);
void build_response(struct http_request *req, int conn);
int set_response(char *str, int conn);

#endif //WEB_SERVER_RESPONSE_H