#include "http_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <unistd.h>

// Structure for HTTP requests, including Route and Handler
struct RouteHandler
{
  char * route;
  char * method;
  void( * handler)(char * request, char * response);
};

// Application structure (App) to be defined by the user
struct App
{
  RouteHandler * routes;
  int route_count, server_socket, new_connection;
  struct sockaddr_in server_address;
};

App * new_app(void) {
  App * a = (App * ) malloc(sizeof(App)); // Allocate memory for the App struct
  if (a == NULL) {
    return NULL;
  }

  a -> route_count = 0;
  a -> routes = NULL;
  a -> server_socket = -1; // Initialize server_socket to an invalid value
  a -> new_connection = -1; // Initialize new_connection to an invalid value
  return a;
}

void generate_http_response(char * response, int status_code,
  const char * body) {
  // Generate the HTTP response
  snprintf(response, 1024, "HTTP/1.1 %d %s\r\nContent-Length: %lu\r\n\r\n%s",
    status_code, (status_code == 200 ? "OK" : "Not Found"), (unsigned long) strlen(body), body);
}

void listen_app(App * a, int port) {
  // Create a socket for the server
  a -> server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (a -> server_socket == -1) {
    perror("Server socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Bind the socket to the specified port
  a -> server_address.sin_family = AF_INET;
  a -> server_address.sin_addr.s_addr = INADDR_ANY;
  a -> server_address.sin_port = htons(port);
  if (bind(a -> server_socket, (struct sockaddr * ) & a -> server_address, sizeof(a -> server_address)) == -1) {
    perror("Server socket binding failed");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(a -> server_socket, 1) == -1) {
    perror("Server socket listening failed");
    exit(EXIT_FAILURE);
  }
  printf("HTTP Server listening on port %d...\n", port);

  // Accept incoming connections and handle them
  while (1) {
    // Accept a new connection
    a -> new_connection = accept(a -> server_socket, NULL, NULL);
    if (a -> new_connection == -1) {
      // If there is an error, continue to the next connection request
      perror("Client connection acceptance failed");
      continue;
    }

    // Receive the request
    char request[MAX_REQUEST_SIZE];
    ssize_t bytes_received = recv(a -> new_connection, request, sizeof(request) - 1, 0);

    // Check for errors
    if (bytes_received < 0) {
      // If there is an error, close the connection
      perror("Request receive error");
      close(a -> new_connection);
      continue;
    }

    // Check if the connection was closed by the client
    if (bytes_received == 0) {
      close(a -> new_connection);
      continue;
    }
    request[bytes_received] = '\0'; // Add a null terminator to the end of the request

    // Generate the response
    char response[MAX_RESPONSE_SIZE];
    char * method = strtok(request, " "); // Get the HTTP method (e.g., "GET", "POST")
    char * route_start = strtok(NULL, " "); // Get the request route (e.g., "/example");

    // Check for errors in the request method and route
    if (method == NULL || route_start == NULL) {
      // If there is an error, send a 400 response
      generate_http_response(response, 400, "Bad Request");
      send(a -> new_connection, response, strlen(response), 0);
      close(a -> new_connection);
      continue;
    }

    int not_found_handler = 1;
    // loop through the registered routes and find a matching one
    for (int i = 0; i < a -> route_count; ++i) {
      RouteHandler handler = a -> routes[i];
      if (strcmp(method, handler.method) == 0 && strcmp(route_start, handler.route) == 0) {
        handler.handler(request, response);
        not_found_handler = 0;
      }
    }

    // If no matching route was found, send a 404 response
    if (not_found_handler) {
      generate_http_response(response, 404, "Not Found");
    }

    // Send the response to the client
    send(a -> new_connection, response, strlen(response), 0);
    close(a -> new_connection);
  }
}

// Function to add a new route handler
void add_route_handler(App * app, char * method, RouteHandler handler) {
  // Increment the current route count and allocate memory
  app -> route_count++;
  app -> routes = (RouteHandler * ) realloc(app -> routes, app -> route_count * sizeof(RouteHandler));

  // Update the method field of the last added RouteHandler
  app -> routes[app -> route_count - 1].route = handler.route;
  app -> routes[app -> route_count - 1].method = method;
  app -> routes[app -> route_count - 1].handler = handler.handler;
}

// Function to add a new GET route
void add_GET_route(App * a, RouteHandler handler) {
  // Add the route handler with the GET method
  add_route_handler(a, "GET", handler);
}

// Function to add a new POST route
void add_POST_route(App * a, RouteHandler handler) {
  // Add the route handler with the POST method
  add_route_handler(a, "POST", handler);
}

// Function to add a new PUT route
void add_PUT_route(App * a, RouteHandler handler) {
  // Add the route handler with the PUT method
  add_route_handler(a, "PUT", handler);
}

// Function to add a new PATCH route
void add_PATCH_route(App * a, RouteHandler handler) {
  // Add the route handler with the PATCH method
  add_route_handler(a, "PATCH", handler);
}

// Function to add a new DELETE route
void add_DELETE_route(App * a, RouteHandler handler) {
  // Add the route handler with the DELETE method
  add_route_handler(a, "DELETE", handler);
}