#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_REQUEST_SIZE 1024
#define MAX_RESPONSE_SIZE 1024

// Structure for HTTP requests, including Route and Handler
typedef struct {
    char *route;
    char *method;
    void (*handler)(char *request, char *response);
} RouteHandler;

// Application structure (App) to be defined by the user
typedef struct {
    RouteHandler *routes;
    int route_count, server_socket, new_connection;
    struct sockaddr_in server_address;
} App;

App *new_app(void);
void generate_http_response(char *response, int status_code, const char *body);
void listen_app(App *a, int port);
void add_GET_route(App *a, RouteHandler handler);
void add_POST_route(App *a, RouteHandler handler);
void add_PUT_route(App *a, RouteHandler handler);
void add_PATCH_route(App *a, RouteHandler handler);
void add_DELETE_route(App *a, RouteHandler handler);

#endif /* HTTP_SERVER_H */
       