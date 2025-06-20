#include "../common/message.h"
#include "../common/socket.h"
#include "args.h"
#include "socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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
  Socket *server_socket =
      create_server_socket("INADDR_ANY", 8080, config.max_connections);

  while (1) {
    printf("Waiting for a connection...\n");
    Socket *client_socket = accept_client_connection(server_socket);
    if (!client_socket)
      continue;

    // Read the message from the client
    clear_socket_buffer(client_socket);
    int b_read = receive_message(client_socket);
    if (b_read < 0) {
      close_socket(client_socket);
      continue;
    }
    Message message = get_message(client_socket);

    /* decription */

    // Send acknowledgment back to the client
    if (1) {
      // If all goes well
      enum MessageType msg_type = ACK;
      enum AckType ack_type = ACK_OK;
      clear_socket_buffer(client_socket);
      add_message(client_socket, &msg_type, sizeof(enum MessageType));
      add_message(client_socket, &ack_type, sizeof(enum AckType));

      send_message(client_socket);
    }

    // Close the client socket
    close_socket(client_socket);
    sleep(100);
  }
  return 0;
}
