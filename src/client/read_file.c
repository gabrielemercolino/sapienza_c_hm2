#include "read_file.h"
#include <stdio.h>
#include <stdlib.h>

Text get_text(const char *filename) {
    Text t;
    t.length = -1;

    FILE *file = fopen(filename, "rb");
    if (!file) return t;

    fseek(file, 0, SEEK_END);
    t.length = ftell(file);
    fseek(file, 0, SEEK_SET);

    t.text = malloc(t.length + 1);
    size_t bytes_read = fread(t.text, 1, t.length, file);

    if (fclose(file)) {
        free(t.text);
        t.length = -1;
        return t;
    }
    return t;
}

void free_text(Text *t) {
    if (t->text) {
        free(t->text);
        t->text = NULL;
    }
}