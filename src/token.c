#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <ctype.h>

TokenDef token_defs[3] = {
    {TOK_IDENTIFIER,"^[a-zA-Z_][a-zA-Z0-9_]*"},
    {TOK_NUMBER, "^[0-9]+"},
    {TOK_WHITESPACE, "^[ \t]+"}
};

TokenDef keyword_defs[] = {
    {TOK_KEYWORD_IF, "if"},
    {TOK_KEYWORD_WHILE, "while"},
    {TOK_KEYWORD_FOR, "for"},
    {TOK_KEYWORD_THEN, "then"},
    {TOK_KEYWORD_END, "end"},
};

TokenDef operator_defs[] = {
    {TOK_OPERATOR_EQUALS, "=="},
    {TOK_OPERATOR_PLUS, "+"},
    {TOK_OPERATOR_MINUS, "-"},
    {TOK_OPERATOR_MULTIPLY, "*"},
    {TOK_OPERATOR_DIVIDE, "/"},
    {TOK_OPERATOR_MODULO, "%"},
    {TOK_OPERATOR_GREATER, ">"},
    {TOK_OPERATOR_LESSER, "<"},
    {TOK_OPERATOR_LESSER_EQUALS, "<="},
    {TOK_OPERATOR_GREATER_EQUALS, ">="},
    {TOK_OPERATOR_NOT, "!"},
    {TOK_OPERATOR_NOT_EQUALS, "!="},
    {TOK_OPERATOR_AND, "&&"},
    {TOK_OPERATOR_OR, "||"},
};

int token_defs_len = (sizeof(token_defs)/sizeof(token_defs[0]));
int keyword_defs_len = (sizeof(keyword_defs)/sizeof(keyword_defs[0]));
int operator_defs_len = (sizeof(operator_defs)/sizeof(operator_defs[0]));

void detect_pattern_token(int token_def_index,regmatch_t match,char* p,int *match_len,TokenType* best_match){
    if (regexec(&token_defs[token_def_index].re, p, 1, &match, 0) == 0 && match.rm_so == 0){
        int len = match.rm_eo - match.rm_so;
        if (len > *match_len){
            *match_len = len;
        }
    }
    *best_match = token_defs[token_def_index].token_type;
}

void detect_literal_token(char* target,int* best_len,TokenDef* lookup_table,int lookup_len,TokenType* best_match){
    char word[16];
    sscanf(target,"%15s", word);

    for (int i = 0;i < lookup_len;i++){
        if (strcmp(lookup_table[i].pattern,word) == 0){
            *best_len = strlen(word);
            *best_match = lookup_table[i].token_type;
        }
    }
}
char* token_type_str(TokenType token_type){
    switch(token_type){
        case TOK_KEYWORD:
            return "keyword";
        case TOK_KEYWORD_IF:
            return "keyword if";
        case TOK_KEYWORD_THEN:
            return "keyword then";
        case TOK_KEYWORD_END:
            return "keyword end";
        case TOK_KEYWORD_WHILE:
            return "keyword while";
        case TOK_KEYWORD_FOR:
            return "keyword for";
        case TOK_IDENTIFIER:
            return "identifier";
        case TOK_NUMBER:
            return "number";
        case TOK_OPERATOR:
            return "operator";
        case TOK_OPERATOR_EQUALS:
            return "operator equals";
        case TOK_OPERATOR_PLUS:
            return "operator plus";
        case TOK_OPERATOR_MINUS:
            return "operator minus";
        case TOK_OPERATOR_MULTIPLY:
            return "operator multiply";
        case TOK_OPERATOR_DIVIDE:
            return "operator divide";
        case TOK_OPERATOR_MODULO:
            return "operator modulo";
        case TOK_OPERATOR_GREATER:
            return "operator greater";
        case TOK_OPERATOR_GREATER_EQUALS:
            return "operator greater equals";
        case TOK_OPERATOR_LESSER:
            return "operator lesser";
        case TOK_OPERATOR_LESSER_EQUALS:
            return "operator lesser equals";
        case TOK_OPERATOR_NOT:
            return "operator not";
        case TOK_OPERATOR_NOT_EQUALS:
            return "operator not equals";
        case TOK_OPERATOR_AND:
            return "operator and";
        case TOK_OPERATOR_OR:
            return "operator or";
        case TOK_LPARAN:
            return "left paranthesis";
        case TOK_RPARAN:
            return "right paranthesis";
        case TOK_LBRACE:
            return "left brace";
        case TOK_RBRACE:
            return "right brace";
        case TOK_WHITESPACE:
            return "whitespace";
        case TOK_UNIDENTIFIED:
            return "unidentified";
        default:
            return "unidentified";
    }
}
