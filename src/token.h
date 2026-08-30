#ifndef TOKEN_H
#define TOKEN_H

#include <regex.h>


typedef enum {
    TOK_KEYWORD,
    TOK_KEYWORD_LET,
    TOK_KEYWORD_RETURN,
    TOK_KEYWORD_INT,
    TOK_KEYWORD_FLOAT,
    TOK_KEYWORD_STRING,
    TOK_KEYWORD_BOOL,
    TOK_KEYWORD_TRUE,
    TOK_KEYWORD_FALSE,
    TOK_KEYWORD_FUNCTION,
    TOK_KEYWORD_IF,
    TOK_KEYWORD_ELSE,
    TOK_KEYWORD_THEN,
    TOK_KEYWORD_END,
    TOK_KEYWORD_WHILE,
    TOK_KEYWORD_TO,
    TOK_KEYWORD_BY,
    TOK_KEYWORD_FOR,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_BOOL,
    TOK_FLOAT,
    TOK_INTEGER,
    TOK_STRING,
    TOK_OPERATOR,
    TOK_OPERATOR_ASSIGN,

    TOK_OPERATOR_PLUS,
    TOK_OPERATOR_MINUS,
    TOK_OPERATOR_MULTIPLY,
    TOK_OPERATOR_DIVIDE,
    TOK_OPERATOR_MODULO,

    TOK_OPERATOR_PLUS_EQUALS,
    TOK_OPERATOR_INCREMENT,
    TOK_OPERATOR_MINUS_EQUALS,
    TOK_OPERATOR_DECREMENT,
    TOK_OPERATOR_MULTIPLY_EQUALS,
    TOK_OPERATOR_DIVIDE_EQUALS,
    TOK_OPERATOR_MODULO_EQUALS,

    TOK_OPERATOR_EQUALS,
    TOK_OPERATOR_GREATER,
    TOK_OPERATOR_GREATER_EQUALS,
    TOK_OPERATOR_LESSER,
    TOK_OPERATOR_LESSER_EQUALS,
    TOK_OPERATOR_NOT_EQUALS,

    TOK_OPERATOR_AND,
    TOK_OPERATOR_OR,
    TOK_OPERATOR_NOT,

    TOK_LPARAN,
    TOK_RPARAN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_COMMA,
    TOK_NEWLINE,
    TOK_COLON,
    TOK_UNIDENTIFIED,
    TOK_WHITESPACE,
    TOK_EOF,
}TokenType;

typedef struct Token{
    TokenType token_type;
    const char* start;
    size_t size;

    size_t line;
    size_t column;
}Token;

typedef struct TokenDef{
    TokenType token_type;
    char* pattern;
    regex_t re;
}TokenDef;

typedef struct TokenArray{
    Token* token_array;
    int count;
    int size;
    char *source;
    char *name;
}TokenArray;

extern TokenDef token_defs[3];
extern TokenDef keyword_defs[];
extern TokenDef operator_defs[];
extern TokenDef delimiter_defs[];

extern int token_defs_len;
extern int keyword_defs_len;
extern int operator_defs_len;
extern int delimiter_defs_len;

int is_arithemeticop(TokenType token);
int is_comparisionalop(TokenType token);
int is_logicalop(TokenType token);

void detect_number_token(char* p,int* match_len,TokenType* best_match);
void detect_string_token(char* p,int* match_len,TokenType* best_match);
TokenType lookup_keyword(char* target,size_t best_len);
int check_op(TokenArray* token,int index,TokenType match);
int is_keyword(TokenType token);
int is_binaryop(TokenType token);
void detect_pattern_token(int token_def_index,regmatch_t match,char* p,int *match_len,TokenType* best_match);
void detect_literal_token(char* target,int* best_len,TokenDef* lookup_table,int lookup_len,TokenType* best_match);
char* token_type_str(TokenType token_type);
void push(TokenArray *token_array,Token new_token);
void array_init(TokenArray *token_array);
void print_array(TokenArray *token_array);
char* token_value(Token token);

#endif
