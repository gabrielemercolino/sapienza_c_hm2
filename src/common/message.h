#pragma once
#include "socket.h"

enum MessageType { ENC_MSG = 0, ACK = 1 };

enum AckType {
  ACK_UNKNOWN = -1,
  ACK_OK = 0,
  ACK_ERROR = 1,
  ACK_POOL_FAILED = 2
};

typedef struct {
  uint16_t org_len; // Length of the original message
  uint16_t enc_len; // Length of the encrypted message
  void *msg;        // Pointer to the message data
  uint64_t key;     // Key for encryption/decryption
} Message;

/**
 * @brief Get message in the buffer.
 * @param socket Socket which contains message.
 * @return message structure.
 */
Message get_message(Socket *socket);

/**
 * @brief Get ack in the buffer.
 * @param socket Socket which contains ack.
 * @return ack type.
 */
enum AckType get_ack_type(Socket *socket);
