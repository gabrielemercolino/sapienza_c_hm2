#include "message.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

Message get_message(Socket *socket) {
  Message message = {0};

  uint8_t *cursor = (uint8_t *)socket->buffer;
  enum MessageType msg_type = (enum MessageType) * cursor;
  cursor += sizeof(enum MessageType);

  // Check msg type
  if (msg_type != ENC_MSG) {
    fprintf(stderr, "Expect a message\n");
    free(message.msg);
    message.msg = NULL;
    return message;
  } else if (socket->buffer_size < 1 + sizeof(uint16_t) * 2) {
    fprintf(stderr, "Incomplete message");
    free(message.msg);
    message.msg = NULL;
    return message;
  }

  // Read original length
  memcpy(&message.org_len, cursor, sizeof(uint16_t));
  cursor += sizeof(uint16_t);

  // Read encrypted length
  memcpy(&message.enc_len, cursor, sizeof(uint16_t));
  cursor += sizeof(uint16_t);
  if (socket->buffer_size <
      1 + sizeof(uint16_t) * 2 + sizeof(uint64_t) + message.enc_len) {
    fprintf(stderr, "Incomplete message\n");
    free(message.msg);
    message.msg = NULL;
    return message;
  }

  // Read encrypted message
  message.msg = malloc(message.enc_len);
  memcpy(message.msg, cursor, message.enc_len);
  cursor += message.enc_len;

  // Read key
  memcpy(&message.key, cursor, sizeof(uint64_t));

  // Print
  printf("Original Length: %hu\n", message.org_len);
  printf("Encrypted length: %hu\n", message.enc_len);
  printf("Key: %lu\n", message.key);

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
