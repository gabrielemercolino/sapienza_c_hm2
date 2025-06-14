//
// Created by Pc on 14.06.2025.
//

#ifndef DECRYPT_H
#define DECRYPT_H
#include <stddef.h>

#endif //DECRYPT_H

#define BLOCK_SIZE 8  // 64 bit

typedef struct {
    const char *ciphertext;
    char *plaintext;
    const char *key;
    size_t index;
} DecryptTask;

void decrypt_message(const char *ciphertext, size_t padded_len, const char *key, char *output, int n_threads) ;



