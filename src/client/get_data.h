#pragma once
#include <stddef.h>

/**
 * @brief Reads file content.
 * @param filename file to read.
 * @param length pointer to store the length of the content.
 * @return content.
 */
char *get_data(const char *filename, size_t *length);
