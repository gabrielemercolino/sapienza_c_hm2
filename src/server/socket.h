#include <stdint.h>
#include <stddef.h>

typedef struct {
  uint16_t length;
  char *buffer;
  uint64_t key;
} Message;

/**
 * @brief Creates a server socket and binds it to the specified address and port.
 * @param ip_str The IP address to bind the socket to, or "INADDR_ANY" for any address.
 * @param port The port to bind the socket to.
 * @param max_connections The maximum number of connections to allow.
 * @return The server socket file descriptor, or -1 on error.
 */
int create_server_socket(const char *ip, uint16_t port, uint16_t max_connections);

/**
 * @brief Accepts a client connection on the server socket.
 * @param server_socket The server socket file descriptor.
 * @return The client socket file descriptor, or -1 on error.
 */
int accept_client_connection(int server_socket);

/**
 * @brief Reads a message from the client socket.
 * @param connfd The client socket file descriptor.
 * @return The message read from the socket.
 */
Message read_message(int connfd);

/**
 * @brief Sends an acknowledgment back to the client.
 * @param connfd The client socket file descriptor.
 */
void send_ack(int connfd);
