#ifndef TOKEN_H
#define TOKEN_H

#include <regex.h>


typedef enum {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    LPARAN,
    RPARAN,
    LBRACE,
    RBRACE,
    UNIDENTIFIED,
    WHITESPACE,
}TokenType;

typedef struct Token{
    TokenType token_type;
    char* value;
}Token;

typedef struct TokenDef{
    TokenType token_type;
    char* pattern;
    regex_t re;
}TokenDef;

char* token_type_str(TokenType token_type);
int tokenize(char* p);
void init();
#endif
