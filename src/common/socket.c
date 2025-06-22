#include "socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
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
      fprintf(stderr, "Error allocating memory for message buffer: ");
      return OP_ERROR;
    }
    memcpy(socket->buffer, message, message_size);
    socket->buffer_size = message_size;
  } else {
    // Reallocate memory to accommodate the new message
    void *new_buffer =
        realloc(socket->buffer, socket->buffer_size + message_size);
    if (new_buffer == NULL) {
      fprintf(stderr, "Error reallocating memory for message buffer: ");
      clear_socket_buffer(socket);
      return OP_ERROR;
    }
    socket->buffer = new_buffer;
    memcpy((char *)socket->buffer + socket->buffer_size, message, message_size);
    socket->buffer_size += message_size;
  }

  return OP_OK;
}

OpResult send_message(Socket *socket) {
  // Send message length
  int bytes_write =
      write(socket->fd, &socket->buffer_size, sizeof(socket->buffer_size));
  if (bytes_write < 0) {
    fprintf(stderr, "Error sending message length: ");
    return OP_ERROR;
  }
  // Send message
  bytes_write = write(socket->fd, socket->buffer, socket->buffer_size);
  if (bytes_write < 0) {
    fprintf(stderr, "Error sending buffer data: ");
    return OP_ERROR;
  }
  // Print success message
  printf("Sent message successfully (%ld byte)\n", socket->buffer_size);

  return OP_OK;
}

OpResult receive_message(Socket *socket) {
  // Receive message length
  int bytes_read =
      read(socket->fd, &socket->buffer_size, sizeof(socket->buffer_size));
  if (bytes_read < 0) {
    fprintf(stderr, "Error reading message length: ");
    return OP_ERROR;
  }
  // Receive message
  socket->buffer = malloc(socket->buffer_size);
  bytes_read = read(socket->fd, socket->buffer, socket->buffer_size);
  if (bytes_read < 0) {
    fprintf(stderr, "Error reading buffer data: ");
    return OP_ERROR;
  }

  return OP_OK;
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
  free(socket);
}
