#ifndef LEXER_H
#define LEXER_H
#include "token.h"

void lexer_init();
Token tokenize(char* p,int* best_len);

#endif
