#pragma once
#include "../common/socket.h"

/**
 * @brief Creates a server socket, binds it to an address and set the socket to listen.
 * @param ip_str The IP address to bind the socket to, or "INADDR_ANY" for any interface.
 * @param port The port to bind the socket to.
 * @param max_connections The maximum number of connections to allow.
 * @return The server socket created.
 */
Socket *create_server_socket(const char *ip, uint16_t port, uint16_t max_connections);

/**
 * @brief Accepts a client connection.
 * @param server_socket The server socket.
 * @return The client socket connected.
 */
Socket *accept_client_connection(Socket *server_socket);