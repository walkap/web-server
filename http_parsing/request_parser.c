#include "request_parser.h"

double get_double(char *str) {
    if (str == NULL) return 1.0;
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
    double q;
    strtok(str, s);
    token = strtok(NULL, s);
    q = get_double(token);
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

    //Tokenize the string param and then check if the request is GET or HEAD
    token = strtok(t, sc);
    exit_on_error(t == NULL, "error in strtok in parse_request_line");
    if (strstr(t, "GET") != NULL)
        r->method = "GET";
    if (strstr(token, "HEAD") != NULL)
        r->method = "HEAD";

    //Token the same string and the same param the save the value to the uri struct
    token = strtok(NULL, sc);
    exit_on_error(token == NULL, "error in strtok in parse_request_line");
    r->uri = token;

    //Change the delimiter and get the http version and save it into the version struct
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
void parse_request(char *str, struct http_request *request) {

    const char s[3] = "\r\n";
    char *token, *buff;


    //Set some struct params
    request->alive = 0;
    request->accept = NULL;

    //Allocate enough memory to contain the request string
    buff = malloc(strlen(str) + 1);
    exit_on_error(buff == NULL, "error in malloc");

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
        if (strstr(token, "Accept:") != NULL && strstr(token, "Accept: */*") != 0) {
            request->accept = token;
        }
        //Check for the Connection item then save it into the struct
        if (strstr(token, "Connection: keep-alive") != NULL) {
            request->alive = 1;
        }
        token = strtok(NULL, s);
    }

     //free(buff);
}