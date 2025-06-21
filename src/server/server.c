#include "args.h"
#include "common/message.h"
#include "common/thread_pool.h"
#include "server/decrypt.h"
#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  Socket *client_socket;
  Message *message;

  char *file_prefix;
} ClientHandle;

void handle_client(ClientHandle *handle) {
  Message *message = handle->message;

  size_t padding_length = message->encrypted_len - message->original_len;
  char *decrypted_text = decrypt_message(
      message->encrypted_text, padding_length, message->key, message->threads);

  // write file with padding. Può essere estratto nella funzione separata
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H:%M:%S", tm_info);

  char *fname = strcat(handle->file_prefix, buffer);

  FILE *fout = fopen("../../resources/", "w");
  if (fout == NULL) {
    perror("fopen fail");
  }
  fputs(decrypted_text, fout);

  fclose(fout);
  free(decrypted_text);
  free(fname);

  enum MessageType msg_type = ACK;
  enum AckType ack_type = ACK_OK;
  clear_socket_buffer(handle->client_socket);
  add_message(handle->client_socket, &msg_type, sizeof(enum MessageType));
  add_message(handle->client_socket, &ack_type, sizeof(enum AckType));

  send_message(handle->client_socket);
  close_socket(handle->client_socket);
}

void handle_client_task(void *arg) {
  ClientHandle *handle = arg;

  handle_client(handle);

  free(arg);
}

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

  ThreadPool *pool = create_thread_pool(config.max_connections);

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

    ClientHandle *handle = malloc(sizeof(ClientHandle));
    handle->message = get_message(client_socket);
    handle->client_socket = client_socket;
    handle->file_prefix = config.file_prefix;

    if (thread_pool_try_do(pool, handle_client_task, handle) != STARTED) {
      enum MessageType msg_type = ACK;
      enum AckType ack_type = ACK_POOL_FAILED;
      clear_socket_buffer(client_socket);
      add_message(client_socket, &msg_type, sizeof(enum MessageType));
      add_message(client_socket, &ack_type, sizeof(enum AckType));

      send_message(client_socket);
      close_socket(client_socket);
    }

    sleep(100);
  }
  return 0;
}
