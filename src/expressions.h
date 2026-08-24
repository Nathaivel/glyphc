#include "parser.h"

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

ASTNode* parse_number(TokenArray tokens, int *index);
ASTNode* parse_identifier(TokenArray tokens, int *index);
ASTNode* make_binary_op(ASTNode* left,ASTNode* right, Token operation);
ASTNode* parse_factors(TokenArray tokens,int *index);
ASTNode* parse_terms(TokenArray tokens,int *index);
ASTNode* parse_expression(TokenArray tokens,int *index);
ASTNode* parse_comparision(TokenArray tokens,int *index);
ASTNode* parse_logic(TokenArray tokens,int *index);

#endif
