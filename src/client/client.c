#include "args.h"
#include "read_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  ClientConfig config = {0};
  CPAResult result = client_parse_args(argc, argv, &config);

  if (result == ONLY_HELP) {
    client_print_usage(argv[0]);
    return 0;
  }

  if (result != OK) {
    fprintf(stderr, "%s\n", client_pa_result_to_string(result));
    client_print_usage(argv[0]);
    return 1;
  }

  printf("Hello from client\n");
  printf("Config:\n  -k: %lu\n  -t: %lu\n  -f: '%s'\n  -a: '%s'\n  -p: %hu\n",
         config.key, config.threads, config.file_path, config.server_ip,
         config.server_port);

  // Create socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    perror("Error creating socket");
    return 1;
  }

  // Set up the server address structure
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(config.server_port);
  // Convert the server IP address from string to binary
  if (inet_pton(AF_INET, config.server_ip, &serv_addr.sin_addr) <= 0) {
      close(client_socket);
      perror("Invalid address");
      return 1;
  }

  // Connect to the server
  if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      close(client_socket);
      perror("Connection failed");
      return 1;
  }
  printf("Connected to server %s:%hu\n", config.server_ip, config.server_port);

  // Get file text
  Text text = get_text(config.file_path);
  if (text.length < 0) {
    perror("Error reading file");
    close(client_socket);
    return 1;
  }





  /* Take the text and split it into 64 bit blocks, 
     do xor operation with key using at most p threads,
     thread in encryption can not be interrupted by SIGINT, SIGALRM, SIGUSR1, SIGUSR2, SIGTERM.
     and finally combine these blocks */





  // Send message to the server                                               <-- Write encrypted message here
  int bytes_write = write(client_socket, &text.length, sizeof(text.length));  // replace with encrypted message
  bytes_write += write(client_socket, text.text, strlen(text.text));          // replace with encrypted message length
  bytes_write += write(client_socket, &config.key, sizeof(config.key));
  if (bytes_write < 0) {
    perror("Error writing to socket");
    close(client_socket);
    return 1;
  }
  printf("Sent message successfully\n");
  free_text(&text);

  // Receive message from the server
  char ack[64];
  int bytes_read = read(client_socket, ack, 64);
  if (bytes_read < 0) 
    perror("Error reading from socket");
  else
    printf("Ack from server: %s\n",ack);

  // Close connection
  close(client_socket);
  return 0;
}
