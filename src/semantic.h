

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    char* name;
    TypeKind type;
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
}Scope;

uint64_t hash_string(char* key,size_t length);
SymbolHashMap init_symbol_table();
void add_symbol_to_table(SymbolHashMap* table,char* new_key,void* value);
void resize_symbol_table(SymbolHashMap* table);
Symbol* get_symbol_from_table(SymbolHashMap* table,char* key);
ASTNode* semantic_analysis(ASTNode* tree);

#endif
