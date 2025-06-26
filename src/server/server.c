#include "args.h"
#include "common/message.h"
#include "common/thread_pool.h"
#include "decrypt.h"
#include "socket.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  Socket *client_socket;
  Message *message;

  char *file_prefix;
  size_t threads;
} ClientHandle;

void generate_filename(char *buffer, size_t buffer_size, const char *prefix);

void handle_client(ClientHandle *handle) {
  Message *message = handle->message;

  char *decrypted_data =
      decrypt_message(message->encrypted_data, message->encrypted_len,
                      message->key, handle->threads);

  char fname[strlen(handle->file_prefix) + 32];
  generate_filename(fname, sizeof(fname), handle->file_prefix);

  FILE *fout = fopen(fname, "w");
  if (fout == NULL) {
    fprintf(stderr, "failed to create file %s\n", fname);

    enum MessageType msg_type = ACK;
    enum AckType ack_type = ACK_ERROR;

    clear_socket_buffer(handle->client_socket);
    add_message(handle->client_socket, &msg_type, sizeof(enum MessageType));
    add_message(handle->client_socket, &ack_type, sizeof(enum AckType));

    send_message(handle->client_socket);
    close_socket(handle->client_socket);
  }

  fputs(decrypted_data, fout);

  fclose(fout);
  free(decrypted_data);

  printf("Saved in %s\n", fname);

  enum MessageType msg_type = ACK;
  enum AckType ack_type = ACK_OK;
  clear_socket_buffer(handle->client_socket);
  add_message(handle->client_socket, &msg_type, sizeof(enum MessageType));
  add_message(handle->client_socket, &ack_type, sizeof(enum AckType));

  send_message(handle->client_socket);
  close_socket(handle->client_socket);
  free(handle->client_socket);
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
  Socket *server_socket = malloc(sizeof(Socket));
  assert(server_socket && "failed to allocate socket");

  SSStatus status = create_server_socket(server_socket, "INADDR_ANY", 8080,
                                         config.max_connections);
  if (status != SS_OK) {
    fprintf(stderr, "%s\n", ss_status_to_string(status));
    free(server_socket);
    return 1;
  }

  ThreadPool *pool = create_thread_pool(config.threads);
  if (!pool) {
    fprintf(stderr, "Error creating thread pool\n");
    close_socket(server_socket);
    free(server_socket);
    return 1;
  }

  while (1) {
    // Allocate on the heap as it will be passed to multiple threads
    Socket *client_socket = malloc(sizeof(Socket));
    assert(client_socket && "failed to allocate memory for the client socket");
    if (!wait_client_connection(server_socket, client_socket)) {
      fprintf(stderr, "Error accepting connection\n");
      free(client_socket);
      continue;
    }

    printf("Accepted connection from client\n");

    // Read the message from the client
    clear_socket_buffer(client_socket);
    OpResult res = receive_message(client_socket);

    if (res != OP_MESSAGE_RECEIVED) {
      fprintf(stderr, "%s\n", op_result_to_string(res));
      close_socket(client_socket);
      continue;
    }

    Message *message = get_message(client_socket);
    if (message == NULL) {
      enum MessageType msg_type = ACK;
      enum AckType ack_type = ACK_ERROR;

      clear_socket_buffer(client_socket);
      add_message(client_socket, &msg_type, sizeof(enum MessageType));
      add_message(client_socket, &ack_type, sizeof(enum AckType));

      send_message(client_socket);
      close_socket(client_socket);
    }

    // allocate on the heap as it's passed to multiple threads
    ClientHandle *handle = malloc(sizeof(ClientHandle));
    assert(handle && "failed to allocate memory for the request handle");
    handle->message = message;
    handle->client_socket = client_socket;
    handle->file_prefix = config.file_prefix;
    handle->threads = config.threads;

    if (thread_pool_try_do(pool, handle_client_task, handle) != STARTED) {
      enum MessageType msg_type = ACK;
      enum AckType ack_type = ACK_POOL_FAILED;
      clear_socket_buffer(client_socket);
      add_message(client_socket, &msg_type, sizeof(enum MessageType));
      add_message(client_socket, &ack_type, sizeof(enum AckType));

      send_message(client_socket);
      close_socket(client_socket);
      free(client_socket);
    }
  }
  return 0;
}

void generate_filename(char *buffer, size_t buffer_size, const char *prefix) {
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);

  char time_str[32];
  strftime(time_str, sizeof(time_str), "%Y%m%d_%H%M%S", tm_info);

  snprintf(buffer, buffer_size, "%s%s.txt", prefix, time_str);
}
