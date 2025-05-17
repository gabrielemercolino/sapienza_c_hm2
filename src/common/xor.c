#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// XOR operation between two strings
char* xor(const char *input, const char *key) {
    if (strlen(input) != 64) {
        fprintf(stderr, "input must be long 64 bit\n");
        return NULL;
    }
    if (strlen(key) != 64) {
        fprintf(stderr, "key must be long 64 bit\n");
        return NULL;
    }
    char *result = malloc(65);

    // XOR
    for (int i=0; i<64; i++) {
        if (input[i] == '1' && key[i] == '1') {
            result[i] = '0';
        } else if (input[i] == '0' && key[i] == '0') {
            result[i] = '0';
        } else {
            result[i] = '1';
        }
    }
    result[64] = '\0';

    return result;
}