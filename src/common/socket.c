#include "socket.h"
#include "message.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

Socket *create_client_socket(const char *server_ip, unsigned short server_port) {
  // Create socket
  Socket *client_socket = malloc(sizeof(Socket));
  client_socket->fd = socket(AF_INET, SOCK_STREAM, 0);
  client_socket->buffer = NULL;
  if (client_socket->fd < 0) {
    fprintf(stderr, "Error creating socket: ");
    return client_socket;
  }

  // Set server address and port
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
    fprintf(stderr, "Invalid address: ");
    client_socket->fd = -1;
    return client_socket;
  }

  // Connect to the server
  if (connect(client_socket->fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    // Refuse connection if the server is not available
    if (errno == ECONNREFUSED) {
      fprintf(stderr, "Connection refused by server %s:%hu: ", server_ip, server_port);
    } else {
      fprintf(stderr, "Error connecting to server %s:%hu: ", server_ip, server_port);
    }
    client_socket->fd = -1;
    return client_socket;
  }
  printf("Connected to server %s:%hu\n", server_ip, server_port);

  return client_socket;
}

Socket *create_server_socket(const char *ip, uint16_t port, uint16_t max_connections) {
  // Create socket
  Socket *server_socket = malloc(sizeof(Socket));
  server_socket->fd = socket(AF_INET, SOCK_STREAM, 0);
  server_socket->buffer = NULL;
  if (server_socket->fd < 0) {
    fprintf(stderr, "Error creating socket: ");
    server_socket->fd = -1;
    return server_socket;
  }

  // Set up the server address structure
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (strcmp(ip, "INADDR_ANY") == 0) {
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
      perror("Invalid IP address");
      close(server_socket->fd);
      server_socket->fd = -1;
      return server_socket;
    }
  }

  // Bind the socket to the address
  if (bind(server_socket->fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error binding socket");
    close(server_socket->fd);
    server_socket->fd = -1;
    return server_socket;
  }

  // Set the socket to listen for incoming connections
  if (listen(server_socket->fd, max_connections) < 0) {
    perror("Error listening on socket");
    close(server_socket->fd);
    server_socket->fd = -1;
  }

  return server_socket;
}

Socket *accept_client_connection(Socket *server_socket) {
  Socket *client_socket = malloc(sizeof(Socket));
  client_socket->buffer = NULL;

  // Accept a client connection
  client_socket->fd = accept(server_socket->fd, (struct sockaddr *)NULL, NULL);
  if (client_socket->fd < 0) {
    fprintf(stderr, "Error accepting connection: ");
    return client_socket;
  }
  printf("Accepted connection from client\n");

  return client_socket;
}

int clear_socket_buffer(Socket *socket) {
  if (socket->buffer) {
    free(socket->buffer);
    socket->buffer = NULL;
    socket->buffer_size = 0;
  }
  return 0;
}

int add_message(Socket *socket, void *message, size_t message_size) {
  // Allocate memory for the message
  if (socket->buffer == NULL) {
    socket->buffer = malloc(message_size);
    if (socket->buffer == NULL) {
      fprintf(stderr, "Error allocating memory for message buffer: ");
      return 1;
    }
    memcpy(socket->buffer, message, message_size);
    socket->buffer_size = message_size;
  } else {
    // Reallocate memory to accommodate the new message
    void *new_buffer = realloc(socket->buffer, socket->buffer_size + message_size);
    if (new_buffer == NULL) {
      fprintf(stderr, "Error reallocating memory for message buffer: ");
      clear_socket_buffer(socket);
      return 1;
    }
    socket->buffer = new_buffer;
    memcpy((char *)socket->buffer + socket->buffer_size, message, message_size);
    socket->buffer_size += message_size;
  }

  return 0;
}

int send_message(Socket *socket) {
  // Send message length
  int bytes_write = write(socket->fd, &socket->buffer_size, sizeof(socket->buffer_size));
  if (bytes_write < 0) {
    fprintf(stderr, "Error sending message length: ");
    return 1;
  }
  // Send message
  bytes_write = write(socket->fd, socket->buffer, socket->buffer_size);
  if (bytes_write < 0) {
    fprintf(stderr, "Error sending buffer data: ");
    return 1;
  }
  // Print success message
  printf("Sent message successfully (%ld byte)\n", socket->buffer_size);

  return 0;
}

int receive_message(Socket *socket) {
  // Receive message length
  int bytes_read = read(socket->fd, &socket->buffer_size, sizeof(socket->buffer_size));
  if (bytes_read < 0) {
    fprintf(stderr, "Error reading message length: ");
    return 1;
  }
  // Receive message
  socket->buffer = malloc(socket->buffer_size);
  bytes_read = read(socket->fd, socket->buffer, socket->buffer_size);
  if (bytes_read < 0) {
    fprintf(stderr, "Error reading buffer data: ");
    return 1;
  }

  return 0;
}

void close_socket(Socket *socket) {
  if (!socket) return;
  
  if (socket->fd >= 0) {
    close(socket->fd);
  }
  if (socket->buffer) {
    free(socket->buffer);
    socket->buffer = NULL;
    socket->buffer_size = 0;
  }
  free(socket);
}