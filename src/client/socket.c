#include "socket.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

Socket *create_client_socket(const char *server_ip,
                             unsigned short server_port) {
  // Create socket
  Socket *client_socket = malloc(sizeof(Socket));
  client_socket->fd = socket(AF_INET, SOCK_STREAM, 0);
  client_socket->buffer = NULL;
  if (client_socket->fd < 0) {
    fprintf(stderr, "Error creating socket: ");
    return NULL;
  }

  // Set server address and port
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
    fprintf(stderr, "Invalid address: ");
    client_socket->fd = -1;
    return NULL;
  }

  // Connect to the server
  if (connect(client_socket->fd, (struct sockaddr *)&serv_addr,
              sizeof(serv_addr)) < 0) {
    // Refuse connection if the server is not available
    if (errno == ECONNREFUSED) {
      fprintf(stderr, "Connection refused by server %s:%hu: ", server_ip,
              server_port);
    } else {
      fprintf(stderr, "Error connecting to server %s:%hu: ", server_ip,
              server_port);
    }
    client_socket->fd = -1;
    return NULL;
  }
  printf("Connected to server %s:%hu\n", server_ip, server_port);

  return client_socket;
}
