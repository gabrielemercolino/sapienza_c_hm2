#include <stdint.h>
#include <stddef.h>

// Structure for client socket information.
typedef struct {
  int fd;          // File descriptor for the client socket
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
 * @param org_length The length of the original message.
 * @param enc_length The length of the encrypted message.
 * @param enc_msg The encrypted message to send.
 * @param key The key for decryption.
 * @return The number of bytes written to the socket. -1 on error.
 */
int send_message(ClientSocket *client_socket, uint16_t org_length, uint16_t enc_length, 
                 void *enc_msg, uint64_t key);

/**
 * @brief Receive an acknowledgment from the server.
 * @param client_socket The client socket.
 * @param ack Pointer to store the acknowledgment value.
 * @return The number of bytes read from the socket. -1 on error.
 */
int receive_ack(ClientSocket *client_socket, uint16_t *ack);

/**
 * @brief Close the socket.
 * @param client_socket The client socket to close.
 */
void close_socket(ClientSocket *client_socket);