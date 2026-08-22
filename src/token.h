#ifndef TOKEN_H
#define TOKEN_H

#include <regex.h>


typedef enum {
    TOK_KEYWORD,
    TOK_KEYWORD_IF,
    TOK_KEYWORD_THEN,
    TOK_KEYWORD_END,
    TOK_KEYWORD_WHILE,
    TOK_KEYWORD_FOR,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_OPERATOR,
    TOK_OPERATOR_EQUALS,
    TOK_OPERATOR_PLUS,
    TOK_OPERATOR_MINUS,
    TOK_OPERATOR_MULTIPLY,
    TOK_OPERATOR_DIVIDE,
    TOK_OPERATOR_MODULO,
    TOK_OPERATOR_GREATER,
    TOK_OPERATOR_GREATER_EQUALS,
    TOK_OPERATOR_LESSER,
    TOK_OPERATOR_LESSER_EQUALS,
    TOK_OPERATOR_NOT,
    TOK_OPERATOR_NOT_EQUALS,
    TOK_OPERATOR_AND,
    TOK_OPERATOR_OR,
    TOK_LPARAN,
    TOK_RPARAN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_UNIDENTIFIED,
    TOK_WHITESPACE,
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

extern TokenDef token_defs[3];
extern TokenDef keyword_defs[];
extern TokenDef operator_defs[];

extern int token_defs_len;
extern int keyword_defs_len;
extern int operator_defs_len;

void detect_pattern_token(int token_def_index,regmatch_t match,char* p,int *match_len,TokenType* best_match);
void detect_literal_token(char* target,int* best_len,TokenDef* lookup_table,int lookup_len,TokenType* best_match);
char* token_type_str(TokenType token_type);


#endif
