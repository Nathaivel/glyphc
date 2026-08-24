#include "lexer.h"
#include "token.h"
#include <ctype.h>

void lexer_init(){
    for (int i = 0;i < token_defs_len;i++){
        regcomp(&(token_defs[i].re),token_defs[i].pattern,REG_EXTENDED);
    }
}

Token tokenize(char* p, int* best_len){
    *best_len = -1;
    TokenType best_match = TOK_UNIDENTIFIED;
    regmatch_t match;

    if (isalpha(p[0]) || p[0] == '_'){
        int len = 0;

        while(isalnum(p[len]) || p[len] == '_') len++;
        *best_len = len;
        best_match = lookup_keyword(p, len);

        //detect_literal_token(p, best_len,keyword_defs,keyword_defs_len, &best_match);

        if (best_match == TOK_UNIDENTIFIED){
            //detect_pattern_token(0, match, p, best_len, &best_match);
            best_match = TOK_IDENTIFIER;
        }
    }else if (isdigit(p[0])){
        detect_pattern_token(1, match, p, best_len, &best_match);
    }else if(p[0] == ' '){
        *best_len = -1;
        best_match = TOK_WHITESPACE;
    }else if(p[0] == '\n'){
        *best_len = 1;
        best_match = TOK_NEWLINE;
    }else if(p[0] == '(' || p[0] == ')' || p[0] == '{' || p[0] == '}' || p[0] == '\0'){
        detect_literal_token(p, best_len, delimiter_defs, delimiter_defs_len, &best_match);
    }
    else{
        detect_literal_token(p, best_len, operator_defs,operator_defs_len, &best_match);
    }

    Token token = {best_match,p,*best_len};

    return token;
}

Token peek(char* p, int* jump_len){
    Token token = tokenize(p,jump_len);
    return token;
}

char* advance(char* p, int jump_len){
    if (jump_len < 0){
        p++;
        return p;
    }
    p += jump_len;
    return p;
}

TokenArray lex(char* p){
    lexer_init();
    TokenArray tokens;
    Token token;
    int return_len;
    array_init(&tokens);

    while(1){
            Token token = peek(p,&return_len);

            if (token.token_type != TOK_WHITESPACE){
                 push(&tokens,token);
            }


            if (token.token_type == TOK_EOF){
                break;
            }
            p = advance(p,return_len);
        }

    print_array(&tokens);
    return tokens;
}
