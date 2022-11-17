#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

typedef enum {
    TYPE_IMPORT = 0,
    TYPE_CLASS,
    TYPE_CLASS_ELEM,
} TSymbolType;


void init_symbol_table();
int add_entry(char * name, TSymbolType type);
void destroy_symbol_table();
void new_scope();
void end_scope();
#endif