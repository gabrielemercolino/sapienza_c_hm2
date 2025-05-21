#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

ClientSocket create_socket(const char *server_ip, unsigned short server_port) {
  // Create a socket
  ClientSocket client_socket;
  client_socket.fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket.fd < 0) {
    perror("Error creating socket");
    return client_socket;
  }

  // Set server address and port
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
    perror("Invalid address");
    close(client_socket.fd);
    client_socket.fd = -1;
    return client_socket;
  }

  // Connect to the server
  if (connect(client_socket.fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection failed");
    close(client_socket.fd);
    client_socket.fd = -1;
    return client_socket;
  }
  printf("Connected to server %s:%hu\n", server_ip, server_port);

  return client_socket;
}

int send_message(ClientSocket client_socket, uint16_t length, char *enc_msg, uint64_t key) {
  int bytes_write = write(client_socket.fd, &length, sizeof(length));
  bytes_write += write(client_socket.fd, enc_msg, strlen(enc_msg));
  bytes_write += write(client_socket.fd, &key, sizeof(key));
  if (bytes_write <= 0) {
    perror("Error writing to socket");
    close(client_socket.fd);
    return -1;
  }
  printf("Sent message successfully\n");
  printf("Sent length: %hu\n", length);

  free(enc_msg);
  return bytes_write;
}

int receive_ack(ClientSocket client_socket, char *ack_buffer, size_t buffer_size) {
  int bytes_read = read(client_socket.fd, ack_buffer, buffer_size);
  if (bytes_read < 0) {
    perror("Error reading from socket");
    close(client_socket.fd);
    return -1;
  }
  printf("Ack from server: %s\n", ack_buffer);

  return bytes_read;
}

void close_socket(ClientSocket client_socket) {
  if (close(client_socket.fd) < 0) {
    perror("Error closing socket");
  }
}