#include "socket.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

SSStatus create_server_socket(Socket *server_socket, const char *ip,
                              uint16_t port, uint16_t max_connections) {
  // Create socket
  server_socket->fd = socket(AF_INET, SOCK_STREAM, 0);
  server_socket->buffer = NULL;
  server_socket->buffer_size = 0;
  if (server_socket->fd < 0) {
    free(server_socket->buffer);
    return SS_FAILED_CREATION;
  }

  // Set up the server address structure
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (strcmp(ip, "INADDR_ANY") == 0) {
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
      free(server_socket->buffer);
      return SS_INVALID_ADDRESS;
    }
  }

  // set SO_REUSEADDR option to allow reuse of the address
  int opt = 1;
  if (setsockopt(server_socket->fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt))) {
    free(server_socket->buffer);
    return SS_FAILED_REUSEADDR;
  }
  // Bind the socket to the address
  if (bind(server_socket->fd, (struct sockaddr *)&serv_addr,
           sizeof(serv_addr)) < 0) {
    free(server_socket->buffer);
    return SS_FAILED_BINDING;
  }

  // Set the socket to listen for incoming connections
  if (listen(server_socket->fd, max_connections) < 0) {
    free(server_socket->buffer);
    return SS_FAILED_LISTENING;
  }

  return SS_OK;
}

bool accept_client_connection(Socket *server_socket, Socket *client_socket) {
  client_socket->buffer = NULL;
  client_socket->buffer_size = 0;

  // Accept a client connection
  client_socket->fd = accept(server_socket->fd, (struct sockaddr *)NULL, NULL);
  if (client_socket->fd < 0) {
    free(client_socket->buffer);
    free(client_socket);
    return false;
  }

  return true;
}

char *ss_status_to_string(const SSStatus status) {
  switch (status) {
  case SS_OK:
    return "ok";
  case SS_FAILED_CREATION:
    return "failed to create socket";
  case SS_INVALID_ADDRESS:
    return "invalid address";
  case SS_FAILED_REUSEADDR:
    return "failed to allow reuse of address";
  case SS_FAILED_BINDING:
    return "failed to bind socket";
  case SS_FAILED_LISTENING:
    return "failed listening on socket";
  }

  // effectively unreachable but makes compiler happy
  return "invalid status";
}
