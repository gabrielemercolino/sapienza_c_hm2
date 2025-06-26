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

typedef struct {
  char *plaindata;
  char *cipherdata;
  size_t index;
  uint64_t key;
} DecryptTask;

void decrypt_block(void *arg) {
  DecryptTask *task = (DecryptTask *)arg;
  uint64_t *plaindata = (uint64_t *)task->plaindata;
  uint64_t *cipherdata = (uint64_t *)task->cipherdata;

  uint64_t block = cipherdata[task->index];
  uint64_t *out = &plaindata[task->index];

  *out = block ^ task->key;

  free(task);
}

static void signal_handler(int sig) { printf("Ricevuto segnale %d\n", sig); }

char *decrypt_message(char *cipherdata, size_t padded_len, uint64_t key,
                      size_t n_threads) {
  char *output = malloc(padded_len);

  size_t num_blocks = padded_len*8 / BLOCK_SIZE;

  ThreadPool *pool = create_thread_pool(n_threads);

  // per ogni blocco da 64 bit creo decrypt task e avvio il thread pool
  for (size_t i = 0; i < num_blocks; ++i) {
    DecryptTask *task = malloc(sizeof(DecryptTask));

    task->plaindata = output;
    task->cipherdata = cipherdata;
    task->index = i;
    task->key = key;

    if (thread_pool_do(pool, decrypt_block, task) != STARTED) {
      fprintf(stderr, "Error theadpool decrypt task %zu\n", i);
      free(task);
    }
  }

  thread_pool_join(pool);
  thread_pool_free(pool);

  return output;
}
