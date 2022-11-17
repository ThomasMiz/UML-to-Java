#include <stdio.h>
#include<stdlib.h>
#include "buffer.h"
#include <string.h>

#define DIM 200

#define SIZE 200

typedef struct bufferCDT {
    char * buffer;
    ssize_t ptr;
    size_t size;
    char * filename;

} bufferCDT;

static char tmp_buf[200];

bufferADT init_buffer(char * filename) {
    bufferCDT * buffer = malloc(sizeof(bufferCDT));
    buffer->buffer = malloc(DIM);
    buffer->size = DIM;
    int len = strlen(filename);
    char * ext = ".java";
    buffer->filename = malloc(len +strlen(ext) +1);
    strcpy(buffer->filename, filename);
    strcat(buffer->filename, ext);
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
    system("mkdir output 2> /dev/null");
    strcpy(tmp_buf, "rm output/");
    strcat(tmp_buf, buffer->filename);
    strcat(tmp_buf, " 2> /dev/null");
    system(tmp_buf);
    strcpy(tmp_buf, "rm ./");
    strcat(tmp_buf, buffer->filename);
    strcat(tmp_buf, " 2> /dev/null");
    tmp_buf[0] = '.';
    tmp_buf[1] = '/';
    strcpy(tmp_buf + 2, buffer->filename);
    FILE *fp = fopen(tmp_buf, "ab");
    if (fp != NULL)
    {
        fputs(buffer->buffer, fp);
        fclose(fp);
    }
    strcpy(tmp_buf, "java -jar ./formatter/google-java-format-1.15.0-all-deps.jar ./");
    strcat(tmp_buf, buffer->filename);
    strcat(tmp_buf, " > output/");
    strcat(tmp_buf, buffer->filename);
    system(tmp_buf);

    strcpy(tmp_buf, "rm ./");
    strcat(tmp_buf, buffer->filename);
    system(tmp_buf);
}

void destroy_buffer(bufferADT buffer) {
    free(buffer->buffer);
    free(buffer->filename);
    free(buffer);
}

