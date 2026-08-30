#include "symbols.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


uint64_t hash_string(char* key,size_t length){
    uint64_t hash = 14695981039346656037ULL;

    for(int i = 0;i < length;i++){
        hash ^= (unsigned char)*key++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

SymbolHashMap init_symbol_table(){
    SymbolHashMap hash_map;
    hash_map.capacity = 32;
    hash_map.entries = calloc(hash_map.capacity,sizeof(SymbolHashEntry));
    hash_map.size = 0;

    return hash_map;
}

int find_row_in_symbol_table(SymbolHashMap* table,char* new_key){
    int index = hash_string(new_key, strlen(new_key)) % table->capacity;

    while (table->entries[index].occupied){
        if (strcmp(table->entries[index].name,new_key) == 0) return index;

        index = (index + 1)%table->capacity;
    }

    return index;
}

void resize_symbol_table(SymbolHashMap* table){
    size_t old_entries_count = table->capacity;
    SymbolHashEntry* old_entries = table->entries;

    table->capacity *= 2;
    table->entries = calloc(table->capacity, sizeof(SymbolHashEntry));

    if (table->entries == NULL){
        fprintf(stdout,"reallocation failure\n");
        exit(1);
    }

    for (int i = 0;i < old_entries_count;i++){
        if (old_entries[i].occupied){
            add_symbol_to_table(table, old_entries[i].name, old_entries[i].value);
        }
    }
    free(old_entries);
}

void add_symbol_to_table(SymbolHashMap* table,char* new_key,void* value){
    if (table->capacity * 0.7 <= table->size ){
        resize_symbol_table(table);
    }

    int row = find_row_in_symbol_table(table, new_key);

    if (!table->entries[row].occupied){
        table->entries[row].name = new_key;
        table->entries[row].occupied = true;
        table->size++;
    }

    table->entries[row].value = value;
}

Symbol* get_symbol_from_table(SymbolHashMap* table,char* key){
    int row = find_row_in_symbol_table(table, key);

    if (table->entries[row].occupied){
        return table->entries[row].value;
    }

    return NULL;
}
