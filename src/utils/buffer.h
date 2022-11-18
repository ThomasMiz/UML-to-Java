#ifndef BUFFER_H
#define BUFFER_H
typedef struct bufferCDT * bufferADT;

bufferADT init_buffer(char * file_name);
void destroy_buffer(bufferADT buffer);
void generate_file(bufferADT buffer);
void write_buffer(bufferADT buffer, char * s);
char * get_current_string_buffer(bufferADT buffer);

#endif