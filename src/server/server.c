#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  ServerConfig config = {0};
  SPAResult result = server_parse_args(argc, argv, &config);

  if (result == ONLY_HELP) {
    server_print_usage(argv[0]);
    return 0;
  }

  if (result != OK) {
    fprintf(stderr, "%s\n", server_pa_result_to_string(result));
    server_print_usage(argv[0]);
    return 1;
  }

  printf("Hello form server\n");
  printf("Config:\n  -t: %lu\n  -p: '%s'\n  -c: %d\n", config.threads,
         config.file_prefix, config.max_connections);

  // Create socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("Error creating socket");
    return 1;
  }

  // Set up the server address structure
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces
  // Bind the socket to the address
  if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error binding socket");
    return 1;
  }

  // Set the socket to listen for incoming connections
  listen(server_socket, config.max_connections);

  while (1) {
    printf("Waiting for a connection...\n");
    // Accept a connection from a client
    int connfd = accept(server_socket, (struct sockaddr *)NULL, NULL);
    if (connfd < 0) {
      perror("Error accepting connection");
      continue;
    }
    printf("Accepted connection from client\n");

    // Read the message from the client
    uint64_t length;
    int bytes_read = read(connfd, &length, sizeof(length));
    char buffer[length + 1];
    bytes_read += read(connfd, buffer, sizeof(buffer)-1);
    buffer[length] = '\0';
    uint64_t key;
    bytes_read += read(connfd, &key, sizeof(key));
    if (bytes_read < 0) {
      perror("Error reading from socket");
      close(connfd);
      continue;
    }
    printf("Length: %lu\n", length);
    printf("Buffer: %s\n", buffer);
    printf("Key: %lu\n", key);
    





    // parse the message





    // Send acknowledgment back to the client
    char ack[] = "Message received";
    int bytes_sent = write(connfd, ack, strlen(ack));
    if (bytes_sent < 0) {
      perror("Error sending acknowledgment");
    }

    close(connfd);
    sleep(1);
  }

  return 0;
}
