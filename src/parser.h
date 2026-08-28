#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef enum NodeType{
    NODE_BINARY_OP,
    NODE_LITERAL,
    NODE_ASSIGNMENT,
    NODE_DECLARATION,
    NODE_IDENTIFIER,
    NODE_PROGRAM,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_PARAMETERS,
    NODE_FUNCTION_DECLARATION,
    NODE_RETURN,
    NODE_FUNCTION_CALL,
}NodeType;

typedef enum TypeKind{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_NULL
}TypeKind;

typedef struct ASTNode ASTNode;
struct ASTNode{
    NodeType token_type;
    TypeKind type;

    union{
        Token literal;
        Token identifier;
        ASTNode* expression;
        struct{
            Token operation;
            ASTNode* left;
            ASTNode* right;
        } binary_op;
        struct{
            ASTNode** statements;
            int capacity;
            int count;
        } node_list;
        struct{
            ASTNode* identifier;
            ASTNode* expression;
        } assignment;
        struct{
            ASTNode* expression;
            ASTNode* block;
            ASTNode* else_block;
        } if_statement;

        struct{
            ASTNode* identifier;
            ASTNode* parameters;
            ASTNode* block;
        } function;

        struct{
            ASTNode* identifier;
            ASTNode* start;
            ASTNode* stop;
            ASTNode* step;
            ASTNode* block;
        } for_statement;
    } node;
};

ASTNode* parse_function_call(TokenArray tokens,int *index);
ASTNode* create_assignment(TokenArray tokens,int *index);
ASTNode* create_conditional(TokenArray tokens,int *index,ASTNode* conditional_statement);
ASTNode* create_block(TokenArray tokens, int *index,ASTNode* block_pointer);
ASTNode* create_number_node(char* number,size_t size);
void check_token_validity(TokenArray tokens, int *index,TokenType target_type);
void print_statement(ASTNode* program,int depth);
ASTNode* program_init(NodeType token_type);
void parse(char* p);

#endif
