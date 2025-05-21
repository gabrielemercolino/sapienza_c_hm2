#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Reads a text file and returns its content.
 * @param filename The name of the file to read.
 * @return text
 */
char *get_text(const char *filename);