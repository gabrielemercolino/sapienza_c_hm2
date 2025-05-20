#include "socket.h"
#include "read_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int create_socket(const char *server_ip, unsigned short server_port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Error creating socket");
    return -1;
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
    close(sockfd);
    perror("Invalid address");
    return -1;
  }

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    close(sockfd);
    perror("Connection failed");
    return -1;
  }
  printf("Connected to server %s:%hu\n", server_ip, server_port);

  return sockfd;
}

void send_message(int client_socket, uint16_t length, char *enc_msg, uint64_t key) {
  int bytes_write = write(client_socket, &length, sizeof(length));
  bytes_write += write(client_socket, enc_msg, strlen(enc_msg));
  bytes_write += write(client_socket, &key, sizeof(key));
  if (bytes_write < 0) {
    perror("Error writing to socket");
    close(client_socket);
  }
  printf("Sent message successfully\n");
  printf("Sent length: %hu\n", length);
}

void receive_ack(int client_socket, char *ack_buffer, size_t buffer_size) {
  int bytes_read = read(client_socket, ack_buffer, buffer_size);
  if (bytes_read < 0) perror("Error reading from socket");
  printf("Ack from server: %s\n", ack_buffer);
}

void close_socket(int client_socket) {
  if (close(client_socket) < 0) {
    perror("Error closing socket");
  }
}