#include "socket.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

CSStatus create_client_socket(Socket *client_socket, const char *server_ip,
                              unsigned short server_port) {
  // Create socket
  client_socket->fd = socket(AF_INET, SOCK_STREAM, 0);
  client_socket->buffer = NULL;
  client_socket->buffer_size = 0;
  if (client_socket->fd < 0) {
    free(client_socket->buffer);
    return CS_FAILED_CREATION;
  }

  // Set server address and port
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
    free(client_socket->buffer);
    return CS_INVALID_ADDRESS;
  }

  // Connect to the server
  if (connect(client_socket->fd, (struct sockaddr *)&serv_addr,
              sizeof(serv_addr)) < 0) {
    CSStatus error_status;
    // Refuse connection if the server is not available
    if (errno == ECONNREFUSED) {
      error_status = CS_CONNECTION_REFUSED;
    } else {
      error_status = CS_CONNECTION_ERROR;
    }
    free(client_socket->buffer);
    return error_status;
  }

  return CS_OK;
}

char *cs_status_to_string(const CSStatus status) {
  switch (status) {

  case CS_OK:
    return "ok";
  case CS_FAILED_CREATION:
    return "error creating socket";
  case CS_INVALID_ADDRESS:
    return "invalid address";
  case CS_CONNECTION_REFUSED:
    return "connection refused by server";
  case CS_CONNECTION_ERROR:
    return "error connecting to server";
  }

  // effectively unreachable but makes compiler happy
  return "invalid status";
}
