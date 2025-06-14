//
// Created by Pc on 14.06.2025.
//
#include "decrypt.h"

#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "client/args.h"
#include "../../src/common/thread_pool.h"


void decrypt_block(void *arg) {
    DecryptTask *task = (DecryptTask *)arg;

    size_t offset = task->index * BLOCK_SIZE;   //calcolo offset del blocco i-esimo con i==index

    uint64_t cipher_block;
    memcpy(&cipher_block, task->ciphertext + offset, BLOCK_SIZE);   //copio 64 bit dal cipthertext in cipher_block

    uint64_t key_block;
    memcpy(&key_block, task->key, BLOCK_SIZE);  //copio chiave di 64 bit dal task->key in key_block

    //parte logica
    uint64_t plain_block = cipher_block ^ key_block;    //XOR bitwise tra il blocco cifrato e la chiave(xor simmetrico alla cifratura)
    memcpy(task->plaintext + offset, &plain_block, BLOCK_SIZE);     //copio 64 bit di blocco decifrato nel plain text con offset

    free(task);
}

void decrypt_message(const char *ciphertext, size_t padded_len, const char *key, char *output, int n_threads) {
    // blocca solo i segnali specificati
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGALRM);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigaddset(&sigset, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    size_t num_blocks = padded_len / BLOCK_SIZE;

    ThreadPool *pool = create_thread_pool(n_threads);

    //per ogni blocco da 64 bit creo decrypt task e avvio il thread pool
    for (size_t i = 0; i < num_blocks; ++i) {
        DecryptTask *task = malloc(sizeof(DecryptTask));

        task->ciphertext = ciphertext;
        task->plaintext = output;
        task->key = key;
        task->index = i;

        if (thread_pool_do(pool, decrypt_block, task) != 0) {
            fprintf(stderr, "Error theadpool decrypt task %zu\n", i);
            free(task);
        }
    }

    thread_pool_join(pool);
    thread_pool_free(pool);
}