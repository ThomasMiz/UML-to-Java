#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "../backend/support/logger.h"
#include "../frontend/syntactic-analysis/bison-actions.h"
#include "buffer.h"
#define SIZE 500
#define PARAMS_SIZE 50
typedef struct {
    int class_id;
    TSymbolType type;
    char * name;
} table_elem;


table_elem *table;
int dim = 0;
int size = 0;
int current_id = 0;
int last_id = 0;

void init_symbol_table() {
    table = malloc(sizeof(table_elem)*SIZE);
    size = SIZE;
    dim = 0;
    current_id = 0;
    last_id = 0;
}

// static void eliminate_variable_names(TParameterList * params) {
//     while (params != NULL) {
//         *(params->symbolName) = 0;
//         params = params->next;
//     }
// }

void _write_param_types(bufferADT buffer, TParameterList* paramList) {
    while (paramList != NULL) {
        writeTTypeName(buffer, paramList->typeName);
        paramList = paramList->next;
        if (paramList != NULL)
            write_buffer(buffer, ", ");
    }
}

static bufferADT _get_method_full_name_buffer(char * name,TMethodParameterList* param_list) {
  
    bufferADT buffer = init_buffer(NULL);
    write_buffer(buffer, name);
    write_buffer(buffer, "(");
    if (param_list != NULL) {
        // eliminate_variable_names(param_list->parameterList);
        _write_param_types(buffer, param_list->parameterList);
    }
    write_buffer(buffer, ")");

    return buffer;
}

static void _add_entry(table_elem entry) {
    LogDebug("\tadding entry for %s", entry.name);
    if (dim == size) {
        size += SIZE;
        table = realloc(table, size*sizeof(table_elem));
    }
    table[dim++] = entry;
}



static table_elem _get_entry(char * name, TSymbolType type) {
      int len = strlen(name);
    char * s = malloc(len+1);
    strcpy(s, name);
    table_elem entry = {
        .name = s,
        .type = type,
        .class_id = current_id
    };
    return entry;
}

static void _print_entry(table_elem entry) {
    LogDebug("{class_id: %d, name: %s, type: %d}", entry.class_id, entry.name, entry.type);
}

static void _print_table() {
    LogDebug("\tPrinting current table:");
    for (int i=0 ; i<dim ; i++) {
        _print_entry(table[i]);
    }
}



static int _entry_equal(table_elem entry, table_elem otherEntry) {
    return strcmp(entry.name, otherEntry.name) == 0 && entry.class_id == otherEntry.class_id;
}

static int _has_entry(table_elem entry) {
    for (int i=0 ; i<dim ; i++) {
        if (_entry_equal(entry, table[i]))
            return 1;
    }
    return 0;
}

void new_class() {
    last_id++;
    current_id = last_id;
    LogDebug("\tnew class with id = %d", current_id);
}

void end_class() {
    LogDebug("\tend class with id = %d", current_id);
    current_id = 0;
}



int add_entry_method(char * name, TMethodParameterList* param_list) {
    // buscar si la clase se llama igual, si se llama igual es porque es un constructor 
    // entonces agregamos como nombre a los parametros entre parentesis como parte del string
    LogDebug("\ttrying to add entry for method %s", name);
    _print_table();
    
    bufferADT buffer = _get_method_full_name_buffer(name, param_list);
    char * full_name = get_current_string_buffer(buffer);
    LogDebug("full method name is %s", full_name);
    int added = add_entry(full_name, TYPE_CLASS_ELEM);
    // table_elem entry = _get_entry(full_name, TYPE_CLASS_ELEM);
    destroy_buffer(buffer);

    return added;
}





int add_entry(char * name, TSymbolType type) {
    LogDebug("\ttrying to add entry for %s", name);
    table_elem entry = _get_entry(name, type);
    _print_entry(entry);
    _print_table();

    

    if (_has_entry(entry))
        return -1;

    _add_entry(entry);


    return 0;
}

void destroy_symbol_table() {
    for (int i=0 ; i<dim ; i++) {
        free(table[i].name);
    }
}