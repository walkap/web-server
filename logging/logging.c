#include "logging.h"

FILE *open_logfile(char *s) {
    FILE *fptr;
    fptr = fopen(s, "a");
    exit_on_error(fptr == NULL, "error in opening file");

    return fptr;
}

/**
 * This method writes logs about every request and response in the logfile.
 *
 * @param pt
 * @param str
 * @param lenght
 */
void logging(struct http_request *pt, char *str) {
    int rv;
    char *s = " ", *response;
    struct tm tm;
    time_t t;

    s = "\r";
    response = strtok(str, s);

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
            pt->method,
            pt->version,
            pt->uri,
            response);

    rv = fclose(ptr);
    exit_on_error(rv < 0, "error in closing file");
}