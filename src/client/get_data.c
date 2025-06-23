#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "get_data.h"

char *get_data(const char *filename, size_t *length) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Get file size
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("Error getting file stats");
        fclose(file);
        return NULL;
    }
    *length = st.st_size;

    // Read file data
    char *data = malloc(*length);
    if (!data) {
        fclose(file);
        return NULL;
    }
    if (fread(data, 1, *length, file) != *length) {
        perror("Error reading file");
        free(data);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return data;
}
