#pragma once
#include "../common/socket.h"

/**
 * @brief Create a client socket and connect to the server.
 * @param server_ip The IP address of the server.
 * @param server_port The port number of the server.
 * @return The client socket created.
 */
Socket *create_client_socket(const char *server_ip, uint16_t server_port);