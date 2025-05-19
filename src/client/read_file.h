#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct {
  char *text;
  uint64_t length;
} Text;

/**
 * @brief Reads a text file and returns its content.
 * @param filename The name of the file to read.
 * @return A Text struct containing the file content and its length.
 */
Text get_text(const char *filename);

/**
 * @brief Frees the memory allocated for the text in a Text struct.
 * @param t A pointer to the Text struct to free.
 */
void free_text(Text *t);