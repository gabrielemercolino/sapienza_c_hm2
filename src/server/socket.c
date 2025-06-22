#include "socket.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Socket *create_server_socket(const char *ip, uint16_t port,
                             uint16_t max_connections) {
  // Create socket
  Socket *server_socket = malloc(sizeof(Socket));
  server_socket->fd = socket(AF_INET, SOCK_STREAM, 0);
  server_socket->buffer = NULL;
  server_socket->buffer_size = 0;
  if (server_socket->fd < 0) {
    fprintf(stderr, "Error creating socket: ");
    free(server_socket->buffer);
    free(server_socket);
    return NULL;
  }

  // Set up the server address structure
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (strcmp(ip, "INADDR_ANY") == 0) {
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
      perror("Invalid IP address");
      free(server_socket->buffer);
      free(server_socket);
      return NULL;
    }
  }

  // set SO_REUSEADDR option to allow reuse of the address
  int opt = 1;
  if (setsockopt(server_socket->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt failed");
    free(server_socket->buffer);
    free(server_socket);
    return NULL;
  }
  // Bind the socket to the address
  if (bind(server_socket->fd, (struct sockaddr *)&serv_addr,
           sizeof(serv_addr)) < 0) {
    perror("Error binding socket");
    free(server_socket->buffer);
    free(server_socket);
    return NULL;
  }

  // Set the socket to listen for incoming connections
  if (listen(server_socket->fd, max_connections) < 0) {
    perror("Error listening on socket");
    free(server_socket->buffer);
    free(server_socket);
    return NULL;
  }

  return server_socket;
}

Socket *accept_client_connection(Socket *server_socket) {
  Socket *client_socket = malloc(sizeof(Socket));
  client_socket->buffer = NULL;
  client_socket->buffer_size = 0;

  // Accept a client connection
  client_socket->fd = accept(server_socket->fd, (struct sockaddr *)NULL, NULL);
  if (client_socket->fd < 0) {
    fprintf(stderr, "Error accepting connection: ");
    free(client_socket->buffer);
    free(client_socket);
    return NULL;
  }
  printf("Accepted connection from client\n");

  return client_socket;
}