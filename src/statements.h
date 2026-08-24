#include "parser.h"
#ifndef STATEMENTS_H
#define STATEMENTS_H

void append_statement(ASTNode** program, ASTNode* statement);
ASTNode* parse_for_statement(TokenArray tokens,int *index);
ASTNode* parse_while_statement(TokenArray tokens,int *index);
ASTNode* parse_if_statement(TokenArray tokens,int *index);
ASTNode* parse_statement(TokenArray tokens,int *index);

#endif
