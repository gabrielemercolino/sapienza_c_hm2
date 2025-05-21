#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

ServerSocket *create_server_socket(const char *ip, uint16_t port, uint16_t max_connections) {
  // Create socket
  ServerSocket *server_socket;
  server_socket->fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket->fd < 0) {
    perror("Error creating socket");
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

ClientSocket *accept_client_connection(ServerSocket *server_socket) {
  ClientSocket *client_socket = malloc(sizeof(ClientSocket));
  if (client_socket == NULL) {
    perror("Error allocating memory for client socket");
    return NULL;
  }
  client_socket->buffer = NULL;

  client_socket->fd = accept(server_socket->fd, (struct sockaddr *)NULL, NULL);
  if (client_socket->fd < 0) {
    perror("Error accepting connection");
    return client_socket;
  }
  printf("Accepted connection from client\n");

  return client_socket;
}

int read_message(ClientSocket *client_socket) {
  free(client_socket->buffer);
  int bytes_read = read(client_socket->fd, &client_socket->length, sizeof(client_socket->length));

  char *buffer = malloc(client_socket->length + 1);
  bytes_read += read(client_socket->fd, buffer, client_socket->length);
  buffer[client_socket->length] = '\0';
  client_socket->buffer = buffer;

  bytes_read += read(client_socket->fd, &client_socket->key, sizeof(client_socket->key));

  if (bytes_read < 0) {
    perror("Error reading from socket");
    return -1;
  }
  printf("Length: %i\n", client_socket->length);
  printf("Buffer: %s\n", client_socket->buffer);
  printf("Key: %lu\n", client_socket->key);
  
  return bytes_read;
}

int send_ack(ClientSocket *client_socket) {
  // Send acknowledgment back to the client
  char ack[] = "Message received";
  int bytes_sent = write(client_socket->fd, ack, strlen(ack));
  if (bytes_sent < 0) {
    perror("Error sending acknowledgment");
    return -1;
  }
  printf("Sent ack successfully\n");

  return bytes_sent;
}

void close_client_socket(ClientSocket *client_socket) {
  if (client_socket == NULL) return;

  if (client_socket->fd >= 0) {
    close(client_socket->fd);
  }
  if (client_socket->buffer) {
    free(client_socket->buffer);
  }
  free(client_socket);
}