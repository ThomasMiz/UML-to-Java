#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "../backend/support/logger.h"
#define SIZE 500
typedef struct {
    int scope;
    TSymbolType type;
    char * name;
} table_elem;

table_elem *table;
int dim = 0;
int size = 0;
int current_scope = 0;

void init_symbol_table() {
    table = malloc(sizeof(table_elem)*SIZE);
    size = SIZE;
    dim = 0;
    current_scope = 0;
}

static void print_entry(table_elem entry) {
    LogDebug("{scope: %d, name: %s, type: %d}", entry.scope, entry.name, entry.type);
}

static void print_table() {
    LogDebug("\tPrinting current table:");
    for (int i=0 ; i<dim ; i++) {
        print_entry(table[i]);
    }
}



static int entry_equal(table_elem entry, table_elem otherEntry) {
    return strcmp(entry.name, otherEntry.name) == 0 && entry.scope == otherEntry.scope;
}

static int has_entry(table_elem entry) {
    for (int i=0 ; i<dim ; i++) {
        if (entry_equal(entry, table[i]))
            return 1;
    }
    return 0;
}

void new_scope() {
    LogDebug("\t new scope");
    current_scope++;
}

void end_scope() {
    LogDebug("\t end scope");
    current_scope--;
}

int add_entry(char * name, TSymbolType type) {
    LogDebug("\tadding entry for %s", name);
    
    int len = strlen(name);
    char * s = malloc(len+1);
    strcpy(s, name);
    table_elem entry = {
        .name = s,
        .type = type,
        .scope = current_scope
    };
    print_entry(entry);
    print_table();

    

    if (has_entry(entry))
        return -1;

    if (dim == size) {
        size += SIZE;
        table = realloc(table, size*sizeof(table_elem));
    }
    table[dim++] = entry;
    return 0;
}

void destroy_symbol_table() {
    for (int i=0 ; i<dim ; i++) {
        free(table[i].name);
    }
}