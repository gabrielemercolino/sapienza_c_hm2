#pragma once

#include "common/socket.h"

#include <stdint.h>

typedef enum {
  SS_OK,
  SS_FAILED_CREATION,
  SS_INVALID_ADDRESS,
  SS_FAILED_REUSEADDR,
  SS_FAILED_BINDING,
  SS_FAILED_LISTENING
} SSStatus;

/**
 * @brief Creates a server socket, binds it to an address and set the socket to
 * listen.
 * @param socket the socket to setup
 * @param ip_str The IP address to bind the socket to, or "INADDR_ANY" for any
 * interface.
 * @param port The port to bind the socket to.
 * @param max_connections The maximum number of connections to allow.
 * @return The status.
 */
SSStatus create_server_socket(Socket *server_socket, const char *ip,
                              uint16_t port, uint16_t max_connections);

/**
 * @brief Accepts a client connection.
 * @param server_socket The server socket.
 * @param client_socket The client socket if it is accepted.
 * @return wheter the client connection was entablished or not.
 */
bool wait_client_connection(Socket *server_socket, Socket *client_socket);

/**
 * @brief Returns the string representation of the SSStatus passed
 * @param status
 * @return the string representation
 */
char *ss_status_to_string(const SSStatus status);
