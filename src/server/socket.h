#include <stdint.h>
#include <stddef.h>

// Structure for client socket information.
typedef struct {
  int fd;           // File descriptor for the client socket
  uint16_t length;  // Length of the message
  void *buffer;     // Buffer to hold the message
  uint64_t key;     // Key for encryption
} ClientSocket;

// Structure for server socket information.
typedef struct {
  int fd;           // File descriptor for the server socket  
} ServerSocket;

/**
 * @brief Creates a server socket, binds it to an address and set the socket to listen.
 * @param ip_str The IP address to bind the socket to, or "INADDR_ANY" for any interface.
 * @param port The port to bind the socket to.
 * @param max_connections The maximum number of connections to allow.
 * @return The server socket structure.
 */
ServerSocket *create_server_socket(const char *ip, uint16_t port, uint16_t max_connections);

/**
 * @brief Accepts a client connection on the server socket.
 * @param server_socket The server socket file descriptor.
 * @return The client socket structure.
 */
ClientSocket *accept_client_connection(ServerSocket *server_socket);

/**
 * @brief Reads a message from the client socket.
 * @param client_socket The client socket.
 * @return The number of bytes read from the socket. -1 on error.
 */
int read_message(ClientSocket *client_socket);

/**
 * @brief Sends an acknowledgment back to the client.
 * @param client_socket The client socket.
 * @param ack The acknowledgment.
 * @return The number of bytes sent. -1 on error.
 */
int send_ack(ClientSocket *client_socket, uint16_t ack);

/**
 * @brief Closes the client socket.
 * @param client_socket The client socket to close.
 */
void close_client_socket(ClientSocket *client_socket);