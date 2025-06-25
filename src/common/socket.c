#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void clear_socket_buffer(Socket *socket) {
  if (socket->buffer) {
    free(socket->buffer);
    socket->buffer = NULL;
    socket->buffer_size = 0;
  }
}

OpResult add_message(Socket *socket, void *message, size_t message_size) {
  // Allocate memory for the message
  if (socket->buffer == NULL) {
    socket->buffer = malloc(message_size);
    if (socket->buffer == NULL) {
      return OP_FAILED_BUFFER_ALLOC;
    }
    memcpy(socket->buffer, message, message_size);
    socket->buffer_size = message_size;
  } else {
    // Reallocate memory to accommodate the new message
    void *new_buffer =
        realloc(socket->buffer, socket->buffer_size + message_size);
    if (new_buffer == NULL) {
      clear_socket_buffer(socket);
      return OP_FAILED_BUFFER_ALLOC;
    }
    socket->buffer = new_buffer;
    memcpy((char *)socket->buffer + socket->buffer_size, message, message_size);
    socket->buffer_size += message_size;
  }

  return OP_MESSAGE_ADDED;
}

OpResult send_message(Socket *socket) {
  // Send message length
  int bytes_write =
      write(socket->fd, &socket->buffer_size, sizeof(socket->buffer_size));
  if (bytes_write < 0) {
    return OP_FAILED_LENGTH_SEND;
  }
  // Send message
  bytes_write = write(socket->fd, socket->buffer, socket->buffer_size);
  if (bytes_write < 0) {
    return OP_FAILED_BUFFER_SEND;
  }

  return OP_MESSAGE_SENT;
}

OpResult receive_message(Socket *socket) {
  // Receive message length
  int bytes_read =
      read(socket->fd, &socket->buffer_size, sizeof(socket->buffer_size));
  if (bytes_read < 0) {
    return OP_FAILED_LENGTH_RECEIVE;
  }
  // Receive message
  socket->buffer = malloc(socket->buffer_size);
  bytes_read = read(socket->fd, socket->buffer, socket->buffer_size);
  if (bytes_read < 0) {
    return OP_FAILED_BUFFER_RECEIVE;
  }

  return OP_MESSAGE_RECEIVED;
}

void close_socket(Socket *socket) {
  if (!socket)
    return;

  if (socket->fd >= 0) {
    close(socket->fd);
  }
  if (socket->buffer) {
    free(socket->buffer);
  }
}

char *op_result_to_string(const OpResult result) {
  switch (result) {
  case OP_FAILED_LENGTH_SEND:
    return "error sending message length";
  case OP_FAILED_LENGTH_RECEIVE:
    return "error reading message length";
  case OP_FAILED_BUFFER_ALLOC:
    return "error allocating memory for message buffer";
  case OP_FAILED_BUFFER_SEND:
    return "error sending buffer data";
  case OP_FAILED_BUFFER_RECEIVE:
    return "error reading buffer data";
  case OP_MESSAGE_ADDED:
    return "added data to message";
  case OP_MESSAGE_SENT:
    return "message sent successfully";
  case OP_MESSAGE_RECEIVED:
    return "message read successfully";
  }

  return "unknown result";
}
