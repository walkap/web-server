
#ifndef WEB_SERVER_UTILS_H
#define WEB_SERVER_UTILS_H


#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

 char* HTTP_404 = "HTTP/1.1 404 Not Found\nContent-Type: text/html; charset=iso-8859-1\nContent-Length: 193\nConnection: Closed\n\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> <html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL  was not found on this server.</p></body></html>";
 char* HTTP_400 = "HTTP/1.1 400 Bad Request\ntext/html; charset=iso-8859-1\nContent-Length: 298\nConnection: Closed\n\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> <html><head><title>400 Bad Request</title></head><body><h1>Bad Request</h1><p>Your browser sent a request that this server could not understand.</p><p>The request line contained invalid characters following the protocol string.</p></body></html>";
 char* HTTP_INDEX = "HTTP/1.1 200 OK\nContent-Length: 209\nContent-Type: text/html\nConnection: Closed\n\n<!DOCTYPE HTML PUBLIC \\\"-//IETF//DTD HTML 2.0//EN\\\"> <html><head><title>Content Adaptation</title></head><body><h1>Content Adaptation: Home Page</h1><p>Inserire link delle immagini //TODO</p></body></html>";
 char* HTTP_404_1 = "HTTP/1.1 404 Not Found\nConnection: Closed";

void exit_on_error (int condition, const char * message)
{
    if (condition) {
        perror (message) ;
        exit (EXIT_FAILURE) ;
    }
}

#endif //WEB_SERVER_UTILS_H
