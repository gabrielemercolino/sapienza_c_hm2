#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int create_server_socket(const char *ip, uint16_t port, uint16_t max_connections) {
  // Create socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("Error creating socket");
    return -1;
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
      close(server_socket);
      return -1;
    }
  }

  // Bind the socket to the address
  if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error binding socket");
    close(server_socket);
    return -1;
  }

  // Set the socket to listen for incoming connections
  if (listen(server_socket, max_connections) < 0) {
    perror("Error listening on socket");
    close(server_socket);
    return -1;
  }

  return server_socket;
}

int accept_client_connection(int server_socket) {
  int connfd = accept(server_socket, (struct sockaddr *)NULL, NULL);
  if (connfd < 0) {
    perror("Error accepting connection");
    return -1;
  }
  printf("Accepted connection from client\n");

  return connfd;
}

Message read_message(int connfd) {
  Message msg;

  int bytes_read = read(connfd, &msg.length, sizeof(msg.length));

  char *buffer = malloc(msg.length + 1);
  bytes_read += read(connfd, buffer, msg.length);
  buffer[msg.length] = '\0';
  msg.buffer = buffer;

  bytes_read += read(connfd, &msg.key, sizeof(msg.key));

  if (bytes_read < 0) {
    perror("Error reading from socket");
    close(connfd);
    return msg;
  }
  printf("Length: %i\n", msg.length);
  printf("Buffer: %s\n", msg.buffer);
  printf("Key: %lu\n", msg.key);
  
  return msg;
}

void send_ack(int connfd) {
  // Send acknowledgment back to the client
  char ack[] = "Message received";
  int bytes_sent = write(connfd, ack, strlen(ack));
  if (bytes_sent < 0) {
    perror("Error sending acknowledgment");
  }
  printf("Sent ack successfully\n");
}
