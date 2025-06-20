#pragma once
#include <stdint.h>
#include <stddef.h>

// Structure for socket information.
typedef struct {
  int fd;             // File descriptor for the socket
  void *buffer;       // Buffer for reading/writing data
  size_t buffer_size; // Size of the buffer
} Socket;

/**
 * @brief Create a client socket and connect to the server.
 * @param server_ip The IP address of the server.
 * @param server_port The port number of the server.
 * @return The client socket created.
 */
Socket *create_client_socket(const char *server_ip, uint16_t server_port);

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

/**
 * @brief Clear the socket buffer.
 * @param socket The socket to clear.
 * @return 0 on success, 1 on error.
 */
int clear_socket_buffer(Socket *socket);

/**
 * @brief Add message in the buffer.
 * @param socket The socket.
 * @param message message.
 * @param message_size message size.
 * @return 0 on success, 1 on error.
 */
int add_message(Socket *socket, void *message, size_t message_size);

/**
 * @brief Send the message in buffer to the socket connected.
 * @param socket The Socket structure.
 * @return 0 on success, 1 on error.
 */
int send_message(Socket *socket);

/**
 * @brief Read message from the connected socket.
 * @param socket The socket structure.
 * @return 0 on success, 1 on error.
 */
int receive_message(Socket *socket);

/**
 * @brief Close the socket.
 * @param socket The socket to close.
 */
void close_socket(Socket *socket);