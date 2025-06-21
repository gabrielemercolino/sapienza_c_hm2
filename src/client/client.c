#include "args.h"
#include "client/encryption.h"
#include "common/message.h"
#include "get_text.h"
#include "socket.h"

#include <stdio.h>
#include <string.h>

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
  Socket *client_socket =
      create_client_socket(config.server_ip, config.server_port);
  if (client_socket->fd < 0) {
    close_socket(client_socket);
    return 1;
  }

  // Get file text
  char *text = get_text(config.file_path);
  size_t original_len = strlen(text) * 8;

  // the ciphered text can have '/0' inside and that would make
  // strlen function not work properly
  size_t encrypted_len;
  char *encrypted_text = encrypt_file(config.file_path, config.key,
                                      &encrypted_len, config.threads);

  // Send message to the server
  clear_socket_buffer(client_socket);
  enum MessageType msg_type = ENC_MSG;
  add_message(client_socket, &msg_type, sizeof(enum MessageType));
  add_message(client_socket, &original_len, sizeof(original_len));
  add_message(client_socket, &encrypted_len, sizeof(encrypted_len));
  add_message(client_socket, &config.key, sizeof(config.key));
  add_message(client_socket, encrypted_text, encrypted_len / 8);

  int b_send = send_message(client_socket);
  if (b_send < 0) {
    close_socket(client_socket);
    return 1;
  }

  // Receive msg from the server
  clear_socket_buffer(client_socket);
  int b_read = receive_message(client_socket);
  if (b_read < 0) {
    close_socket(client_socket);
    return 1;
  }

  // Process ack received
  enum AckType ack_type = get_ack_type(client_socket);

  // Take action based on received ack
  int flag = 1;
  if (ack_type == ACK_OK) {
    flag = 0;
    printf("OK\n");
  } else if (ack_type == ACK_ERROR) {
    fprintf(stderr, "Error\n");
  } else if (ack_type == ACK_POOL_FAILED) {
    fprintf(stderr, "Error creating thread pool\n");
  } else {
    fprintf(stderr, "Unknow ack type\n");
  }

  // Close connection
  close_socket(client_socket);
  return flag;
}
