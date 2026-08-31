#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "parser.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum{
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
}SymbolKind;

typedef struct Symbol{
    char* name;
    TypeKind type;
    SymbolKind symbol_type;

    struct Symbol** parameters;
    int parameter_count;
} Symbol;

typedef struct {
    char* name;
    void* value;

    bool occupied;
}SymbolHashEntry;

typedef struct {
    SymbolHashEntry *entries;
    size_t size;
    size_t capacity;
}SymbolHashMap;

typedef struct Scope{
    SymbolHashMap table;
    struct Scope* parent;

    bool is_returnable;
    bool has_return;
    TypeKind return_type;
}Scope;

uint64_t hash_string(char* key,size_t length);
SymbolHashMap init_symbol_table();
void add_symbol_to_table(SymbolHashMap* table,char* new_key,void* value);
void resize_symbol_table(SymbolHashMap* table);
Symbol* get_symbol_from_table(SymbolHashMap* table,char* key);

#endif
