//
// Created by Pc on 14.06.2025.
//

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  char *plaintext;
  char *ciphertext;
  size_t index;
  uint64_t key;
} DecryptTask;

char *decrypt_message(char *ciphertext, size_t padded_len, uint64_t key,
                      size_t n_threads);
