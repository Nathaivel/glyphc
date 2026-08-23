#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef enum NodeType{
    NODE_BINARY_OP,
    NODE_LITERAL,
    NODE_ASSIGNMENT,
    NODE_IDENTIFIER,
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
            ASTNode* identifier;
            ASTNode* expression;
        } assignment;
    } node;
};


void parse(char* p);
#endif
