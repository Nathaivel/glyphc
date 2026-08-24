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
}NodeType;

typedef struct ASTNode ASTNode;
struct ASTNode{
    NodeType token_type;
    union{
        Token literal;
        Token identifier;
        struct{
            Token operation;
            ASTNode* left;
            ASTNode* right;
        } binary_op;
        struct{
            ASTNode** statements;
            int capacity;
            int count;
        } program;
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
            ASTNode* start;
            ASTNode* stop;
            ASTNode* step;
            ASTNode* block;
        } for_statement;
    } node;
};

ASTNode* create_assignment(TokenArray tokens,int *index,ASTNode* branch);
ASTNode* create_conditional(TokenArray tokens,int *index,ASTNode* conditional_statement);
ASTNode* create_block(TokenArray tokens, int *index,ASTNode* block_pointer);
ASTNode* create_number_node(char* number,size_t size);
void check_token_validity(TokenArray tokens, int *index,TokenType target_type);
void print_statement(ASTNode* program,int depth);
ASTNode* program_init();
void parse(char* p);

#endif
