#ifndef LEXER_H
#define LEXER_H
#include "token.h"

void init();
int tokenize(char* p,TokenArray *tokens);

#endif
