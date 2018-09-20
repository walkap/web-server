#include "logging.h"

/**
 * This function returns a pointer to the beginning of the log file
 * @param path
 * @return *FILE
 */
FILE *open_logfile(char *path) {
    FILE *fptr;
    fptr = fopen(path, "a");
    exit_on_error(fptr == NULL, "error in opening file");

    return fptr;
}

/**
 * This function writes logs about every request and response in the logfile.
 *
 * @param request
 * @param response
 * @param lenght
 */
void logging(struct http_request *request, char *response) {
    int rv;
    char *s, *response_line;
    struct tm tm;
    time_t t;

    s = "\r";
    response_line = strtok(response, s);

    t = time(NULL);
    tm = *localtime(&t);

    FILE *ptr = open_logfile(LOG_FILE);

    fprintf(ptr, "[%d-%d-%d %d:%d:%d] %s %s %s %s\n",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            request->method,
            request->version,
            request->uri,
            response_line);

    rv = fclose(ptr);
    exit_on_error(rv < 0, "error in closing file");
}