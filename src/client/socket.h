#include <stdint.h>
#include <stddef.h>

/**
 * Structure to hold the client socket information.
 */
typedef struct {
  int fd;
} ClientSocket;

/**
 * @brief Create a socket and connect to the server.
 * @param server_ip The IP address of the server.
 * @param server_port The port number of the server.
 * @return The ClientSocket structure.
 */
ClientSocket *create_socket(const char *server_ip, uint16_t server_port);

/**
 * @brief Send a message to the server.
 * @param client_socket The ClientSocket structure.
 * @param text The message to send.
 * @param length The length of the message.
 * @param key The key for decryption.
 * @return The number of bytes written to the socket. -1 on error.
 * @note The message is sent in the format: length (2 bytes) + message + key (8 bytes).
 */
int send_message(ClientSocket *client_socket, uint16_t length, char *enc_msg, uint64_t key);

/**
 * @brief Receive a message from the server.
 * @param client_socket The ClientSocket structure.
 * @param buffer The buffer to store the received ack.
 * @param buffer_size The size of the buffer.
 * @return The number of bytes read from the socket. -1 on error.
 */
int receive_ack(ClientSocket *client_socket, char *buffer, size_t buffer_size);

/**
 * @brief Close the socket.
 * @param client_socket The ClientSocket structure.
 */
void close_socket(ClientSocket *client_socket);