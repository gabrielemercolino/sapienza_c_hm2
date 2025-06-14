#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client/encryption.h"
#include "server/decrypt.h"

#define BLOCK_SIZE 8
#define KEY_LENGTH sizeof(uint64_t) / sizeof(char)

static uint64_t parse_key(const char *str);

int main() {
  const char *filename = "tests/test.txt";
  uint64_t key = parse_key("emiliano");
  size_t threads = 3;
  size_t length, padding_length;

  char *cipher = encrypt_file(filename, key, &length, &padding_length, threads);

  printf("length:%zu\n", length);

  // stampa carattere per carattere
  // siccome la cifratura potrebbe
  // aggiungere dei caratteri speciali
  // come '/0'
  printf("\"");
  for (size_t i = 0; i < length / 8; i++) {
    printf("%c", cipher[i]);
  }
  printf("\"\n");

  char *decripted =
      decrypt_message(cipher, length + padding_length, key, threads);

  printf("decripted=\"%s\"\n", decripted);

  free(cipher);
  free(decripted);
  return 0;
}

static uint64_t parse_key(const char *str) {
  uint64_t key = 0;

  for (size_t i = 0; i < KEY_LENGTH; ++i) {
    key <<= 8;
    key |= str[i];
  }

  return key;
}
