//
// Created by Pc on 13.06.2025.
//

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  char *plaindata;
  char *cipherdata;
  size_t index;
  uint64_t key;
} EncryptTask;

char *encrypt_file(const char *filename, uint64_t key, 
                            size_t *in_len, size_t *out_len, 
                            size_t threads);
