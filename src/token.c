#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>

TokenDef token_defs[3] = {
    {IDENTIFIER,"^[a-zA-Z_][a-zA-Z0-9_]*"},
    {NUMBER, "^[0-9]+"},
    {WHITESPACE, "^[ \t]+"}
};

TokenDef literal_token_defs[5];
#define token_defs_len (sizeof(token_defs)/sizeof(token_defs[0]))

void detect_pattern_token(int token_def_index,regmatch_t match,char* p,int match_len){
    if (regexec(&token_defs[token_def_index].re, p, 1, &match, 0) == 0 && match.rm_so == 0){
        int len = match.rm_eo - match.rm_so;
    }
}

char* token_type_str(TokenType token_type){
    switch(token_type){
        case KEYWORD:
            return "keyword";
        case IDENTIFIER:
            return "identifier";
        case NUMBER:
            return "number";
        case OPERATOR:
            return "operator";
        case LPARAN:
            return "left paranthesis";
        case RPARAN:
            return "right paranthesis";
        case LBRACE:
            return "left brace";
        case RBRACE:
            return "right brace";
        case WHITESPACE:
            return "whitespace";
        default:
            return "unidentified";
    }
}
void init(){
    for (int i = 0;i < token_defs_len;i++){
        regcomp(&(token_defs[i].re),token_defs[i].pattern,REG_EXTENDED);
    }
}
int tokenize(char* p){
    int best_len = -1;
    TokenType best_match = UNIDENTIFIED;
    regmatch_t match;

    for(int i = 0; i < token_defs_len;i++){
        if (regexec(&token_defs[i].re, p, 1, &match, 0) == 0 && match.rm_so == 0){
            int len = match.rm_eo - match.rm_so;
            if (len > best_len){
                best_len = len;
                best_match = token_defs[i].token_type;
            }
        }
    }

    if (best_len <= 0) return -1;
    char* value = malloc(best_len + 1);
    memcpy(value,p,best_len);
    value[best_len] = '\0';
    printf("TOKEN(TYPE=%s,VALUE='%s')\n",token_type_str(best_match),value);
    free(value);
    return best_len;
}
