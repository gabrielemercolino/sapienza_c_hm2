#pragma once
#include "common/socket.h"

#include <stdint.h>

typedef enum {
  CS_OK,
  CS_FAILED_CREATION,
  CS_INVALID_ADDRESS,
  CS_CONNECTION_REFUSED,
  CS_CONNECTION_ERROR
} CSStatus;

/**
 * @brief Create a client socket and connect to the server.
 * @param socket the socket to setup
 * @param server_ip The IP address of the server.
 * @param server_port The port number of the server.
 * @return The status.
 */
CSStatus create_client_socket(Socket *socket, const char *server_ip,
                              uint16_t server_port);

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
