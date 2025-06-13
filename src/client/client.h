//
// Created by Pc on 13.06.2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <stddef.h>
#include <stdint.h>

#endif //CLIENT_H



typedef struct {
    uint8_t *data;
    size_t size;
    uint64_t key;
    uint8_t *encrypted_data;
} EncryptionBlock;



