#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_
#include "../backend/semantic-analysis/abstract-syntax-tree.h"
typedef enum {
    TYPE_IMPORT = 1,
    TYPE_CLASS = 2,
    TYPE_INTERFACE = 4,
    TYPE_CLASS_ELEM = 8,
} TSymbolType;


void init_symbol_table();
int add_entry(char * name, TSymbolType type);
void destroy_symbol_table();
void new_class();
void end_class();
int add_entry_method(char * name, TMethodParameterList* param_list);
int is_method_constructor(char * name);
int is_valid_extends(const TTypeName* type_name, const TClassType type);
int is_valid_implements(const TCommaSeparatedTypenames* names);
#endif