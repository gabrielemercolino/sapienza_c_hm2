//
// Created by Pc on 13.06.2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encryption.h"
#include "get_text.h"
#include "../../src/common/thread_pool.h"
#define BLOCK_SIZE 8

void xor_encrypt_block(void *arg){
    EncryptTask *task = (EncryptTask *)arg;
    // calcolare l'indirizzo dell'index-esimo blocco da cifrare (di 8 byte==64 bit),
    // prendilo dalla stringa plaintext e interpretalo come un uint64_t* (cioè come un blocco da 64 bit)
    uint64_t *block = (uint64_t *)(task->plaintext + task->index * BLOCK_SIZE);


    uint64_t *out = (uint64_t *)(task->ciphertext + task->index * BLOCK_SIZE);

    *out = *block ^ task->key;

    free(task);
}

char *encrypt_file(const char *filename, uint64_t key, size_t *out_len, size_t threads) {
    char *text = get_text(filename);
    if (!text) {
        return NULL;
    }

    // Calcolo lunghezza e padding
    size_t length = strlen(text);
    *out_len = length;

    // size_t num_blocks = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // size_t padded_len = num_blocks * BLOCK_SIZE;

    size_t padding=BLOCK_SIZE-(length%BLOCK_SIZE);
    if (padding==BLOCK_SIZE) {
        padding=0;
    }

    size_t padded_len = length + padding;
    size_t num_blocks=padded_len/BLOCK_SIZE;

    char *padded_text = calloc(1, padded_len);  // auto padding con '\0'
    memcpy(padded_text, text, length);
    free(text);

    char *ciphertext = malloc(padded_len);
    if (!ciphertext) {
        free(padded_text);
        return NULL;
    }

    ThreadPool *pool = create_thread_pool(threads);

    for (size_t i = 0; i < num_blocks; ++i) {
        EncryptTask *task = malloc(sizeof(EncryptTask));
        task->plaintext = padded_text;
        task->ciphertext = ciphertext;
        task->index = i;
        task->key = key;

        thread_pool_do(pool, xor_encrypt_block, task);
    }

    thread_pool_join(pool);
    thread_pool_free(pool);

    free(padded_text);
    return ciphertext;
}






// int main(int argc, char *argv[]) {
//     const char *filename = "tests/test";
//     char *key_str="emiliano";
//     uint64_t key;
//     memcpy(&key, key_str, BLOCK_SIZE);
//     size_t threads = 3;
//
//     size_t length=172;
//     char *cipher = encrypt_file(filename, key, &length, threads);
//     if (!cipher) {
//         fprintf(stderr, "Errore nella cifratura\n");
//         return 1;
//     }
//
//     // Invia [cipher, length, key] al server qui...
//     printf("%s\n", cipher);
//     free(cipher);
//     return 0;
// }