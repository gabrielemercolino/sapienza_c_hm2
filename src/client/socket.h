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
 * @param client_socket the socket to setup
 * @param server_ip The IP address of the server.
 * @param server_port The port number of the server.
 * @return The status.
 */
CSStatus create_client_socket(Socket *client_socket, const char *server_ip,
                              uint16_t server_port);

/**
 * @brief Returns a string representation of the CSStatus passed
 * @param status
 * @return The string representation
 */
char *cs_status_to_string(const CSStatus status);
