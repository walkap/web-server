#include "request_parser.h"

double get_double(char *str) {
    while (*str && !(isdigit(*str) || ((*str == '-' || *str == '+') && isdigit(*(str + 1)))))
        str++;

    return strtod(str, NULL);
}

/**
 * This method searches for the weight q in a "Accept:" header line
 * @param str
 * @return
 */
double parse_weight(char *str) {
    char *s = "q";
    char *token;

    strtok(str, s);
    token = strtok(NULL, s);

    double q = get_double(token);

    return q;

}
/**
 * @param str
 * @return
 */
char *parse_user_agent(char *str) {

    char *newStr = str;
    while (*(newStr++) != ' ') {}


    return newStr;
}

/**
 * This method parses the request line to search for the method, protocol version and the uri requested
 * @param r
 * @param t
 */
void parse_request_line(struct http_request *r, char *t) {
    char *sc = " ";
    char *token;

    token = strtok(t, sc);
    exit_on_error(t == NULL, "error in strtok in parse_request_line");

    if (strstr(t, "GET") != NULL)
        r->method = "GET";
    if (strstr(token, "HEAD") != NULL)
        r->method = "HEAD";

    token = strtok(NULL, sc);
    exit_on_error(token == NULL, "error in strtok in parse_request_line");

    r->uri = token;

    sc = "\r";

    token = strtok(NULL, sc);
    exit_on_error(token == NULL, "error in strtok in parse_request_line");

    r->version = token;

}

/**
 * This method builds a http_request struct that contains info regarding requests
 * @param str
 * @return
 */
struct http_request *parse_request(char *str) {
    struct http_request *request;
    const char s[3] = "\r\n";
    char *token;

    request = malloc(sizeof(struct http_request));
    exit_on_error(request == NULL, "error in malloc");

    request->alive = 0;
    int len = (int)strlen(str);
    char *buff = malloc((len + 1));
    exit_on_error(buff == NULL, "error in malloc");
    strcpy(buff, str);

    token = strtok(buff, s);
    exit_on_error(token == NULL, "error in strtok in parse_request");

    parse_request_line(request, buff);

    token = strtok(str, s);

    while (token != NULL) {

        if (strstr(token, "User-Agent:") != NULL) {
            request->user_agent = token;
        }
        if (strstr(token, "Accept:") != NULL) {
            request->accept = token;

        }
        if (strstr(token, "Connection: keep-alive") != NULL) {
            request->alive = 1;
        }

        token = strtok(NULL, s);
    }

    return request;

}