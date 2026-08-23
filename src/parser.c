#include "parser.h"
#include "lexer.h"
#include "token.h"

Token peek(char* p, int* jump_len){
    Token token = tokenize(p,jump_len);
    return token;
}

char* advance(char* p, int jump_len){
    if (jump_len < 0){
        //printf("Unidentified token found... Skipping\n");
        p++;
        return p;
    }
    p += jump_len;
    return p;
}

void parse(char* p){
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
}
