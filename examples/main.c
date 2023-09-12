#include <stdio.h>
#include "../http_server.h"

void GET_example(char * request, char * response) {
  // Generate a 200 response
  generate_http_response(response, 200, "Hello Example Page! (GET)");
}

void POST_example(char * request, char * response) {
  // Generate a 200 response
  generate_http_response(response, 200, "Hello Example Page! (POST)");
}

int main(int argc, char *argv[]) {
  // Create a new App
  App * a = new_app();

  // Add some routes
  RouteHandler getHandler;
  getHandler.route = "/example";
  getHandler.handler = GET_example;
  add_GET_route(a, getHandler);

  RouteHandler postHandler;
  postHandler.route = "/example";
  postHandler.handler = POST_example;
  add_POST_route(a, postHandler);

  // Listen for incoming connections
  listen_app(a, 8080);

  // Free allocated memory for routes and the App struct (not reached in this code)
  if (a -> routes) {
    free(a -> routes);
  }
  free(a);

  return EXIT_SUCCESS;
}