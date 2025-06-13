#include "args.h"
#include "socket.h"

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
  ServerSocket *server_socket = create_server_socket("INADDR_ANY", 8080, config.max_connections);

  while (1) {
    printf("Waiting for a connection...\n");
    ClientSocket *client_socket = accept_client_connection(server_socket);
    if (!client_socket) continue;
    

    // Read the message from the client
    int b_read = read_message(client_socket);
    if (b_read < 0) {
      close_client_socket(client_socket);
      continue;
    }




    /* decription */
    



    
    // Send acknowledgment back to the client
    int b_write = send_ack(client_socket);
    if (b_write < 0) {
      close_client_socket(client_socket);
      continue;
    }

    // Close the client socket
    close_client_socket(client_socket);
    sleep(1);
  }
  return 0;
}
