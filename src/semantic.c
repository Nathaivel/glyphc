#include "semantic.h"
#include "parser.h"
#include "token.h"
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

TypeKind semantic_binary_op_analysis(TypeKind left,TypeKind right, Token operation){
    if (left == TYPE_STRING || right == TYPE_STRING){
        fprintf(stdout,"operation %*.s cannot be used on type string",(int)operation.size,operation.start);
        exit(1);
    }

    if (left == TYPE_INT && right == TYPE_INT){
        return TYPE_INT;
    }
    if ((left == TYPE_FLOAT && right == TYPE_FLOAT) || ((left == TYPE_INT && right == TYPE_FLOAT) || (left == TYPE_FLOAT && right == TYPE_INT))){
        return TYPE_FLOAT;
    }

    return TYPE_VOID;
}

TypeKind lookup_identifier_type(ASTNode* expression, Scope* scope){
    char* identifier_name =  token_value(expression->node.identifier);
    Symbol* identifier = get_symbol_from_table(&scope->table,identifier_name);

    if (identifier == NULL){
        fprintf(stdout,"unkown identifier %s\n",identifier_name);
        exit(1);
    }

    return identifier->type;
}

TypeKind semantic_expression_analysis(ASTNode *expression, Scope* scope){
    if (expression->token_type == NODE_LITERAL){
        return expression->type;
    }

    if (expression->token_type == NODE_IDENTIFIER){
        return lookup_identifier_type(expression,scope);
    }

    if (expression->token_type == NODE_BINARY_OP){
        TypeKind left = semantic_expression_analysis(expression->node.binary_op.left,scope);
        TypeKind right = semantic_expression_analysis(expression->node.binary_op.right,scope);
        return semantic_binary_op_analysis(left,right,expression->node.binary_op.operation);
    }

    return TYPE_VOID;
}

void semantic_assignment_analysis(ASTNode* assignment,Scope* scope){
    ASTNode* identifier = assignment->node.assignment.identifier;
    char* identifier_name = token_value(identifier->node.identifier);

    Symbol* variable = get_symbol_from_table(&scope->table, identifier_name);

    if (variable == NULL){
        fprintf(stdout,"Undeclared variable %s\nvariable is declared by variable_name: type = value\n",identifier_name);
        exit(1);
    }

    TypeKind derived_type = semantic_expression_analysis(assignment->node.assignment.expression,scope);

    if (variable->type != derived_type){
        fprintf(stdout,"declared type %s does not match the derived type %s\n",str_of_type(variable->type),str_of_type(derived_type));
        exit(1);
    }
}

void semantic_declaration_analysis(ASTNode*  declaration,Scope* scope){
    ASTNode* identifier = declaration->node.assignment.identifier;
    char* identifier_name = token_value(identifier->node.identifier);
    TypeKind declared_type = declaration->type;

    Symbol* variable = get_symbol_from_table(&scope->table, identifier_name);

    if (variable != NULL){
        fprintf(stdout,"Variable redeclaration\n");
        exit(1);
    }

    TypeKind derived_type = semantic_expression_analysis(declaration->node.assignment.expression,scope);

    if (derived_type != declared_type){
        fprintf(stdout,"declared type %s does not match the derived type %s\n",str_of_type(declared_type),str_of_type(derived_type));
        exit(1);
    }

    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->name = identifier_name;
    symbol->type = declared_type;

    add_symbol_to_table(&scope->table, symbol->name, symbol);
}

ASTNode* semantic_analysis(ASTNode* tree){
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = NULL;
    scope->table = init_symbol_table();


    /*
    printf("tree = %p\n",(void*)tree);
    printf("count = %d\n",tree->node.node_list.count);
    printf("statements = %p\n",tree->node.node_list.statements);
    */

    if (tree->token_type == NODE_PROGRAM){
        ASTNode** statements = tree->node.node_list.statements;

        for (int i = 0;i < tree->node.node_list.count;i++){
            ASTNode* statement = statements[i];
            //printf("statement = %p\n",(void*)statement);

            if(statement == NULL){
                fprintf(stdout,"error no statements\n");
                exit(1);
            }

            if (statement->token_type == NODE_DECLARATION){
                semantic_declaration_analysis(statement, scope);
            }

            if (statement->token_type == NODE_ASSIGNMENT){
                semantic_assignment_analysis(statement, scope);
            }
        }
    }

    return tree;
}
