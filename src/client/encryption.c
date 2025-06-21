//
// Created by Pc on 13.06.2025.
//
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/thread_pool.h"
#include "encryption.h"
#include "get_text.h"

#define BLOCK_SIZE 64

void xor_encrypt_block(void *arg) {
  EncryptTask *task = (EncryptTask *)arg;
  uint64_t *plaintext = (uint64_t *)task->plaintext;
  uint64_t *ciphertext = (uint64_t *)task->ciphertext;

  // calcolare l'indirizzo dell'index-esimo blocco da cifrare (di 8 byte==64
  // bit), prendilo dalla stringa plaintext e interpretalo come un uint64_t*
  // (cioè come un blocco da 64 bit)
  uint64_t block = plaintext[task->index];
  uint64_t *out = &ciphertext[task->index];

  *out = block ^ task->key;

  free(task);
}

static void signal_handler(int sig) { printf("Ricevuto segnale %d\n", sig); }

char *encrypt_file(const char *filename, uint64_t key, size_t *out_len,
                   size_t threads) {
  // blocca solo i segnali specificati
  signal(SIGINT, signal_handler);
  signal(SIGALRM, signal_handler);
  signal(SIGUSR1, signal_handler);
  signal(SIGUSR2, signal_handler);
  signal(SIGTERM, signal_handler);

  char *text = get_text(filename);
  if (!text) {
    return NULL;
  }

  // Calcolo lunghezza e padding
  size_t length = strlen(text) * 8;
  *out_len = length;

  size_t padding = BLOCK_SIZE - (length % BLOCK_SIZE);
  if (padding == BLOCK_SIZE) {
    padding = 0;
  }

  size_t padded_len = length + padding;
  size_t num_blocks = padded_len / BLOCK_SIZE;

  char *padded_text = calloc(1, padded_len); // auto padding con '\0'
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

    if (thread_pool_do(pool, xor_encrypt_block, task) != STARTED) {
      fprintf(stderr, "Error theadpool encrypt task %zu\n", i);
      free(task);
    }
  }

  thread_pool_join(pool);
  thread_pool_free(pool);

  free(padded_text);
  return ciphertext;
}
