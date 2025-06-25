#pragma once
#include <stddef.h>

// Structure for socket information.
typedef struct {
  int fd;             // File descriptor for the socket
  void *buffer;       // Buffer for reading/writing data
  size_t buffer_size; // Size of the buffer
} Socket;

typedef enum {
  OP_FAILED_LENGTH_SEND,
  OP_FAILED_LENGTH_RECEIVE,

  OP_FAILED_BUFFER_ALLOC,
  OP_FAILED_BUFFER_SEND,
  OP_FAILED_BUFFER_RECEIVE,

  OP_MESSAGE_ADDED,
  OP_MESSAGE_SENT,
  OP_MESSAGE_RECEIVED,
} OpResult;

/**
 * @brief Clear the socket buffer.
 * @param socket The socket to clear.
 */
void clear_socket_buffer(Socket *socket);

/**
 * @brief Add message in the buffer.
 * @param socket The socket.
 * @param message message.
 * @param message_size message size.
 * @return operation result: OP_OK on success, OP_ERROR on error.
 */
OpResult add_message(Socket *socket, void *message, size_t message_size);

/**
 * @brief Send the message in buffer to the socket connected.
 * @param socket The Socket structure.
 * @return operation result: OP_OK on success, OP_ERROR on error.
 */
OpResult send_message(Socket *socket);

/**
 * @brief Read message from the connected socket.
 * @param socket The socket structure.
 * @return operation result: OP_OK on success, OP_ERROR on error.
 */
OpResult receive_message(Socket *socket);

/**
 * @brief Close the socket.
 * @param socket The socket to close.
 */
void close_socket(Socket *socket);

char *op_result_to_string(const OpResult result);
