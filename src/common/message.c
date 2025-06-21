#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Message *get_message(Socket *socket) {
  Message *message = malloc(sizeof(Message));

  uint8_t *cursor = (uint8_t *)socket->buffer;
  enum MessageType msg_type = (enum MessageType) * cursor;
  cursor += sizeof(enum MessageType);

  const size_t min_message_size = sizeof(enum MessageType) +
                                  sizeof(message->encrypted_len) +
                                  sizeof(message->original_len);

  // Check msg type
  if (msg_type != ENC_MSG) {
    fprintf(stderr, "Expect a message\n");
    free(message->encrypted_text);
    message->encrypted_text = NULL;
    return message;
  } else if (socket->buffer_size < min_message_size) {
    fprintf(stderr, "Incomplete message");
    free(message->encrypted_text);
    message->encrypted_text = NULL;
    return message;
  }

  // Read original length
  memcpy(&message->original_len, cursor, sizeof(message->original_len));
  cursor += sizeof(message->original_len);

  // Read encrypted length
  memcpy(&message->encrypted_len, cursor, sizeof(message->encrypted_len));
  cursor += sizeof(message->encrypted_len);

  if (socket->buffer_size <
      min_message_size + message->encrypted_len + sizeof(message->key)) {
    fprintf(stderr, "Incomplete message\n");
    free(message->encrypted_text);
    message->encrypted_text = NULL;
    return message;
  }

  // Read encrypted message
  message->encrypted_text = malloc(message->encrypted_len);
  memcpy(message->encrypted_text, cursor, message->encrypted_len);
  cursor += sizeof(message->encrypted_len);

  // Read key
  memcpy(&message->key, cursor, sizeof(message->key));
  cursor += sizeof(message->key);

  // Read threads
  memcpy(&message->threads, cursor, sizeof(message->threads));

  // Print
  printf("Original Length: %zu\n", message->original_len);
  printf("Encrypted length: %zu\n", message->encrypted_len);
  printf("Key: %lu\n", message->key);

  return message;
}

enum AckType get_ack_type(Socket *socket) {
  uint8_t *cursor = (uint8_t *)socket->buffer;
  enum MessageType msg_type = (enum MessageType) * cursor;
  cursor += 1;

  // Check msg type
  if (msg_type != ACK) {
    fprintf(stderr, "Expect an acknowledgement: ");
    return ACK_UNKNOWN;
  } else if (socket->buffer_size < 1 + sizeof(enum AckType)) {
    fprintf(stderr, "Incomplete data: ");
    return ACK_UNKNOWN;
  }

  // Read ack type
  enum AckType ack_type = (enum AckType) * cursor;

  return ack_type;
}
