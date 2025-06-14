#include <stdio.h>
#include <stdlib.h>

#include "get_text.h"

char *get_text(const char *filename) {
  char *text = NULL;

  FILE *file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file");
    return text;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  text = malloc(length + 1);
  size_t bytes_read = fread(text, length - 1, 1, file);

  if (fclose(file)) {
    free(text);
  }
  return text;
}
