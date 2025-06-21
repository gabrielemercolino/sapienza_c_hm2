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
  if (server_socket->fd < 0) {
    fprintf(stderr, "Error creating socket: ");
    server_socket->fd = -1;
    return server_socket;
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
      close(server_socket->fd);
      server_socket->fd = -1;
      return server_socket;
    }
  }

  // Bind the socket to the address
  if (bind(server_socket->fd, (struct sockaddr *)&serv_addr,
           sizeof(serv_addr)) < 0) {
    perror("Error binding socket");
    close(server_socket->fd);
    server_socket->fd = -1;
    return server_socket;
  }

  // Set the socket to listen for incoming connections
  if (listen(server_socket->fd, max_connections) < 0) {
    perror("Error listening on socket");
    close(server_socket->fd);
    server_socket->fd = -1;
  }

  return server_socket;
}

Socket *accept_client_connection(Socket *server_socket) {
  Socket *client_socket = malloc(sizeof(Socket));
  client_socket->buffer = NULL;

  // Accept a client connection
  client_socket->fd = accept(server_socket->fd, (struct sockaddr *)NULL, NULL);
  if (client_socket->fd < 0) {
    fprintf(stderr, "Error accepting connection: ");
    return client_socket;
  }
  printf("Accepted connection from client\n");

  return client_socket;
}
