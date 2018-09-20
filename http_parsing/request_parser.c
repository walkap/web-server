#include "request_parser.h"
#include "../server/server.h"

/**
 * This function finds the quality weight in the accept string
 * @param str
 * @return double
 */
double get_double(char *str) {
    if (str == NULL) return 0.8;
    while (*str && !(isdigit(*str) || ((*str == '-' || *str == '+') && isdigit(*(str + 1)))))
        str++;
    return strtod(str, NULL);
}

/**
 * This function searches for the weight q in a "Accept:" header line
 * @param str
 * @return double
 */
double parse_weight(char *str) {
    char *s = "q";
    char *token;
    double q;
    strtok(str, s);
    token = strtok(NULL, s);
    q = get_double(token);
    return q;
}

/**
 * Separates the user agent from the header line
 * @param str
 * @return char*
 */
char *parse_user_agent(char *str) {
    char *newStr = str;
    while (*(newStr++) != ' ') {}
    return newStr;
}

/**
 * This function parses the request line to search for the method, protocol version and the uri requested
 * @param request
 * @param header_line
 */
void parse_request_line(struct http_request *request, char *header_line) {

    char *p,*end;

    if (strstr(header_line, "GET") != NULL)
        request->method = "GET";
    if (strstr(header_line, "HEAD") != NULL)
        request->method = "HEAD";
    if (strstr(header_line, "HTTP/1.1") != NULL)
        request->version = "HTTP/1.1";
    if (strstr(header_line, "HTTP/1.0") != NULL)
        request->version = "HTTP/1.0";

    p = header_line;
    while (*(p++) != ' ') {}

    end = p;
    while (*(end++) != ' ') {}
    end--;
    *end = '\0';
    request->uri = p;
}

/**
 * This function builds a http_request struct that contains info regarding requests
 * @param str
 */
void parse_request(char *str, struct http_request *request) {

    const char s[3] = "\r\n";
    char *token;
    char buff[MAXLINE];

    //Set some struct params
    request->alive = 0;
    request->accept = NULL;

    //Copy the arg str in the new buffer
    strcpy(buff, str);

    //Tokenize the string copied in the buff (str arg) delimited by s
    token = strtok(buff, s);
    exit_on_error(token == NULL, "error in strtok in parse_request");

    //Parse the request line and save the data in the request struct
    parse_request_line(request, buff);

    //Tokenize the str arg delimited by s
    token = strtok(str, s);

    while (token != NULL) {
        //Check for the User-Agent item then save it into the struct
        if (strstr(token, "User-Agent:") != NULL) {
            request->user_agent = token;
        }
        //Check for the Accept item then save it into the struct
        if (strstr(token, "Accept:") != NULL) {
            request->accept = token;
        }
        //Check for the Connection item then save it into the struct
        if (strstr(token, "Connection: keep-alive") != NULL) {
            request->alive = 1;
        }
        token = strtok(NULL, s);
    }
}