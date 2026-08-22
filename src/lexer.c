#include "lexer.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void init(){
    for (int i = 0;i < token_defs_len;i++){
        regcomp(&(token_defs[i].re),token_defs[i].pattern,REG_EXTENDED);
    }
}

int tokenize(char* p){
    int best_len = -1;
    TokenType best_match = TOK_UNIDENTIFIED;
    regmatch_t match;

    if (isalpha(p[0]) || p[0] == '_'){
        detect_literal_token(p, &best_len,keyword_defs,keyword_defs_len, &best_match);

        if (best_match == TOK_UNIDENTIFIED){
            detect_pattern_token(0, match, p, &best_len, &best_match);
        }
    }else if (isdigit(p[0])){
        detect_pattern_token(1, match, p, &best_len, &best_match);
    }else{
        detect_literal_token(p, &best_len, operator_defs,operator_defs_len, &best_match);
    }

    if (best_len <= 0) return -1;

    char* value = malloc(best_len + 1);
    memcpy(value,p,best_len);
    value[best_len] = '\0';
    printf("TOKEN(TYPE=%s,VALUE='%s')\n",token_type_str(best_match),value);
    free(value);

    return best_len;
}
