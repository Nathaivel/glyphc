#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef enum NodeType{
    NODE_BINARY_OP,
    NODE_LITERAL,
    NODE_ASSIGNMENT,
    NODE_IDENTIFIER,
    NODE_PROGRAM,
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
    } node;
};


void parse(char* p);
ASTNode* parse_expression(TokenArray tokens,int *index);
ASTNode* parse_logic(TokenArray tokens,int *index);
ASTNode* parse_comparision(TokenArray tokens,int *index);
ASTNode* parse_statement(TokenArray tokens,int *index);
ASTNode* parse_factors(TokenArray tokens,int *index);
ASTNode* parse_terms(TokenArray tokens,int *index);
#endif
