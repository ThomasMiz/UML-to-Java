#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "buffer.h"
#include <string.h>

#define DIM 500

#define SIZE 200

typedef struct bufferCDT {
    char * buffer;
    long long ptr;
    size_t size;
    char * filename;

} bufferCDT;

static char tmp_buf[200];

bufferADT init_buffer(char * filename) {
    if (filename == NULL)
        filename = "<EMPTY>";
    bufferCDT * buffer = malloc(sizeof(bufferCDT));
    buffer->buffer = malloc(DIM);
    buffer->size = DIM;
    int len = strlen(filename);
    char * ext = ".java";
    buffer->ptr = 0;
    buffer->filename = malloc(len +strlen(ext) +1);
    strcpy(buffer->filename, filename);
    strcat(buffer->filename, ext);
    return buffer;
}

void write_buffer(bufferADT buffer, char * s) {
    int len = strlen(s);
    if ((buffer->size - buffer->ptr) <= len ) {
        int added = len >= DIM ? 2*len : DIM;
        buffer->size+=added;
        buffer->buffer = realloc(buffer->buffer, buffer->size);
    }

    strcpy(buffer->buffer + buffer->ptr, s);
    buffer->ptr += len;
}

char * get_current_string_buffer(bufferADT buffer) {
    return buffer->buffer;
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
    system(tmp_buf);
    strcpy(tmp_buf, "./");
    strcat(tmp_buf, buffer->filename);
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

