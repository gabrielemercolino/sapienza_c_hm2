//
// Created by Pc on 13.06.2025.
//
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/thread_pool.h"
#include "encryption.h"
#include "get_data.h"

#define BLOCK_SIZE 64

void xor_encrypt_block(void *arg) {
  EncryptTask *task = (EncryptTask *)arg;
  uint64_t *plaindata = (uint64_t *)task->plaindata;
  uint64_t *cipherdata = (uint64_t *)task->cipherdata;

  // calcolare l'indirizzo dell'index-esimo blocco da cifrare (di 8 byte==64
  // bit), prendilo dalla stringa plaindata e interpretalo come un uint64_t*
  // (cioè come un blocco da 64 bit)
  uint64_t block = plaindata[task->index];
  uint64_t *out = &cipherdata[task->index];

  *out = block ^ task->key;

  free(task);
}

char *encrypt_file(const char *filename, uint64_t key, 
                   size_t *in_len, size_t *out_len, size_t threads) {
  char *data = get_data(filename, in_len);
  if (!data) {
    return NULL;
  }

  // Calcolo lunghezza e padding
  size_t bit_length = *in_len *8;

  size_t padding = BLOCK_SIZE - (bit_length % BLOCK_SIZE);
  if (padding == BLOCK_SIZE) {
    padding = 0;
  }

  size_t padded_len = bit_length + padding;
  *out_len = padded_len / 8;
  size_t num_blocks = padded_len / BLOCK_SIZE;

  char *padded_data = calloc(1, *out_len); // auto padding con '\0'
  memcpy(padded_data, data, *in_len);
  free(data);

  char *cipherdata = malloc(*out_len);
  if (!cipherdata) {
    free(padded_data);
    return NULL;
  }

  ThreadPool *pool = create_thread_pool(threads);

  for (size_t i = 0; i < num_blocks; ++i) {
    EncryptTask *task = malloc(sizeof(EncryptTask));
    task->plaindata = padded_data;
    task->cipherdata = cipherdata;
    task->index = i;
    task->key = key;

    if (thread_pool_do(pool, xor_encrypt_block, task) != STARTED) {
      fprintf(stderr, "Error theadpool encrypt task %zu\n", i);
      free(task);
    }
  }

  thread_pool_join(pool);
  thread_pool_free(pool);

  free(padded_data);
  
  return cipherdata;
}
