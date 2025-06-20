#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *data;
  size_t size;
  uint64_t key;
  uint8_t *encrypted_data;
} EncryptionBlock;
