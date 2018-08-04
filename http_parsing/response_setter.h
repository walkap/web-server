#ifndef WEB_SERVER_RESPONSE_H
#define WEB_SERVER_RESPONSE_H

#include "request_parser.h"
#include "../utils.h"

char *set_get_response(struct http_request* req)
{
    char *response;
    if (strcmp(req -> uri, "/index.html")==0) {
        response = HTTP_INDEX;
    }
    else{
        response = HTTP_404;
    }

    return response;
}

char *set_response(char *str)
{
      struct http_request* request;
      char *response;
      request = parse_request(str);

    if (strcmp(request -> method, "GET")==0) {
          response = set_get_response(request);
      }
      else if (strcmp(request -> method, "HEAD")==0) {
          response = HTTP_404;
      }
      else{
          response = HTTP_400;
      }

      free(request);

      return response;

}

#endif //WEB_SERVER_RESPONSE_H
