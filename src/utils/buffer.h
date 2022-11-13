#ifndef BUFFER_H
#define BUFFER_H
typedef struct bufferCDT * bufferADT;

void generate_file(bufferADT buffer);
void write_buffer(bufferADT buffer, char * s);
bufferADT init_buffer(char * file_name);
void destroy_buffer(bufferADT buffer);
#endif