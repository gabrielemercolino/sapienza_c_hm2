//
// Created by Pc on 13.06.2025.
//

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#endif //ENCRYPTION_H


#include <stddef.h>
#include <stdint.h>


typedef struct {
    char *plaintext;
    char *ciphertext;
    size_t index;
    uint64_t key;
} EncryptTask;


char *encrypt_file(const char *filename, uint64_t key, size_t *out_len, size_t threads) ;