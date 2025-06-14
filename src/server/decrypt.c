//
// Created by Pc on 14.06.2025.
//
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/thread_pool.h"
#include "decrypt.h"

#define BLOCK_SIZE 64

void decrypt_block(void *arg) {
  DecryptTask *task = (DecryptTask *)arg;
  uint64_t *plaintext = (uint64_t *)task->plaintext;
  uint64_t *ciphertext = (uint64_t *)task->ciphertext;

  uint64_t block = ciphertext[task->index];
  uint64_t *out = &plaintext[task->index];

  *out = block ^ task->key;

  free(task);
}

static void signal_handler(int sig) { printf("Ricevuto segnale %d\n", sig); }

char *decrypt_message(char *ciphertext, size_t padded_len, uint64_t key,
                      size_t n_threads) {
  // blocca solo i segnali specificati
  signal(SIGINT, signal_handler);
  signal(SIGALRM, signal_handler);
  signal(SIGUSR1, signal_handler);
  signal(SIGUSR2, signal_handler);
  signal(SIGTERM, signal_handler);

  char *output = malloc(padded_len / 8);

  size_t num_blocks = padded_len / BLOCK_SIZE;

  printf("padding_len=%zu\n", padded_len);
  printf("num_blocks=%zu\n", num_blocks);

  ThreadPool *pool = create_thread_pool(n_threads);

  // per ogni blocco da 64 bit creo decrypt task e avvio il thread pool
  for (size_t i = 0; i < num_blocks; ++i) {
    DecryptTask *task = malloc(sizeof(DecryptTask));

    task->plaintext = output;
    task->ciphertext = ciphertext;
    task->index = i;
    task->key = key;

    if (thread_pool_do(pool, decrypt_block, task) != OK) {
      fprintf(stderr, "Error theadpool decrypt task %zu\n", i);
      free(task);
    }
  }

  thread_pool_join(pool);
  thread_pool_free(pool);

  return output;
}
