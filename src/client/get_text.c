#include <stdio.h>
#include <stdlib.h>

#include "get_text.h"

char *get_text(const char *filename) {
  char *text = NULL;

  FILE *file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  fseek(file, 0, SEEK_SET);

  text = malloc(length + 1);
  if (!text) {
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(text, 1, length, file);
  fclose(file);

  if (bytes_read != length) {
    free(text);
    return NULL;
  }

  text[length] = '\0';
  return text;
}
