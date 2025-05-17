#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convert a string to a binary string
char* string_to_binary(const char *input) {
    if (input == NULL) return NULL;

    int len = strlen(input);
    char *result = malloc(len*8+1);

    for (int i = 0; i < len; i++) {
        unsigned char c = input[i];
        for (int j = 0; j < 8; j++) {
            // Use the mask to check each bit
            unsigned char mask = 0b10000000 >> j;
            result[i*8+j] = (c & mask) ? '1' : '0';
        }
    }
    result[len*8] = '\0';

    return result;
}

// Convert a binary string to a string
char* binary_to_string(const char* binary_str) {
    if (binary_str == NULL) return NULL;

    // Check if the length of the binary string is a multiple of 8
    int bin_len = strlen(binary_str);
    if (bin_len%8 != 0) {
        fprintf(stderr, "length must be multiple of 8\n");
        return NULL;
    }

    int char_count = bin_len / 8;
    char* result = malloc(char_count+1);

    for (int i = 0; i<char_count; i++) {
        // Check 8 bits at a time
        char byte_str[9];
        strncpy(byte_str, binary_str+i*8, 8);
        byte_str[8] = '\0';

        // Check if the byte_str is valid
        for (int j=0; j<8; j++) {
            if (byte_str[j] != '0' && byte_str[j] != '1') {
                fprintf(stderr, "error: '%c'\n", byte_str[j]);
                free(result);
                return NULL;
            }
        }

        // Convert the byte_str to a character
        unsigned char byte = 0;
        for (int j=0; j<8; j++) {
            // byte_str[j]-'0': difference between ASCII value
            byte = (byte << 1) | (byte_str[j] - '0');
        }
        result[i] = (char)byte;
    }
    result[char_count] = '\0';

    return result;
}

// Convert a binary string (64 bit) to a long long int
long long int binary_to_lli(const char *binary) {
    if (strlen(binary) != 64) {
        printf("binary must long 64 bit\n");
        return 0;
    }

    long long int result = 0;
    for (int i = 0; i < 64; i++) {
        // Shift result left by 1 bit and add the current bit
        if (binary[i] == '1') {
            result = (result << 1) | 1;
        } else if (binary[i] == '0') {
            result <<= 1;
        } else {
            printf("error: '%c'\n", binary[i]);
            return 0;
        }
    }
    return result;
}

// Convert a long long int to a binary string (64 bit)
char* lli_to_binary(long long int value) {
    char *binary = malloc(65);
    if (binary == NULL) return NULL;

    for (int i = 0; i < 64; i++) {
        binary[i] = (value & (1ULL << (63 - i))) ? '1' : '0';
    }
    binary[64] = '\0';

    return binary;
}

/*
int main() {
    char input[] = "emiliano";
    char *binary = string_to_binary(input);
    char *str = binary_to_string(binary);
    long long int lli = binary_to_lli(binary);
    char *binary_from_lli = lli_to_binary(lli);
    if (binary) {
        printf("Binary string: %s\n", binary);
        printf("String: %s\n", str);
        printf("Long long int: %lld\n", lli);
        printf("Binary from long long int: %s\n", binary_from_lli);
        free(binary);
        free(str);
        free(binary_from_lli);
    }
    return 0;
}
*/