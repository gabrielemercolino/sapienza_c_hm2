//
// Created by Pc on 14.06.2025.
//

#pragma once

#include <stddef.h>
#include <stdint.h>

char *decrypt_message(char *ciphertext, size_t padded_len, uint64_t key,
                      size_t n_threads);
