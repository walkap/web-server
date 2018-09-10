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
void logging(struct http_request *pt, char *str, size_t lenght) {
    int rv;
    char *s = " ";
    char *response;

    strtok(str, s);
    s = "\r";
    response = strtok(NULL, s);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char *user_agent = parse_user_agent(pt->user_agent);

    FILE *ptr = open_logfile("../logfile");

    fprintf(ptr, "%s %s %s  %d-%d-%d %d:%d:%d   %s   %s    %lu\n", pt->method,
            pt->uri, pt->version, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            user_agent, response, lenght);

    rv = fclose(ptr);
    exit_on_error(rv < 0, "error in closing file");
}