#include <stdio.h>
#include<stdlib.h>
#include "buffer.h"
#include <string.h>

#define DIM 200

typedef struct bufferCDT {
    char * buffer;
    ssize_t ptr;
    size_t size;
    char * filename;
} bufferCDT;

bufferADT init_buffer(char * filename) {
    bufferCDT * buffer = malloc(sizeof(bufferCDT));
    buffer->buffer = malloc(DIM);
    buffer->size = DIM;
    int len = strlen(filename);
    buffer->filename = malloc(len + 1);
    strcpy(buffer->filename, filename);
    return buffer;
}

void write_buffer(bufferADT buffer, char * s) {
    int len = strlen(s);
    if (buffer->size + len) {
        int added = len > DIM ? 2*len : DIM;
        buffer->size+=added;
        buffer->buffer = realloc(buffer->buffer, buffer->size);
    }

    strcpy(buffer->buffer + buffer->ptr, s);
    buffer->ptr += len;
}

void generate_file(bufferADT buffer) {
    printf("%s: ", buffer->filename);
    printf("%s", buffer->buffer);
}

void destroy_buffer(bufferADT buffer) {
    free(buffer->buffer);
    free(buffer->filename);
    free(buffer);
}

