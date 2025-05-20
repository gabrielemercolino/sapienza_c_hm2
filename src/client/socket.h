#include <stdint.h>
#include <stddef.h>

/**
 * @brief Create a socket and connect to the server.
 * @param server_ip The IP address of the server.
 * @param server_port The port number of the server.
 * @return The socket file descriptor on success, or -1 on failure.
 */
int create_socket(const char *server_ip, uint16_t server_port);

/**
 * @brief Send a message to the server.
 * @param client_socket The socket file descriptor.
 * @param text The message to send.
 * @param length The length of the message.
 * @param key The key to send with the message.
 */
void send_message(int client_socket, uint16_t length, char *enc_msg, uint64_t key);

/**
 * @brief Receive a message from the server.
 * @param sockfd The socket file descriptor.
 * @param buffer The buffer to store the received message.
 * @param buffer_size The size of the buffer.
 */
void receive_ack(int client_socket, char *buffer, size_t buffer_size);

/**
 * @brief Close the socket.
 * @param client_socket The socket file descriptor.
 */
void close_socket(int client_socket);