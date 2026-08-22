#include "token.h"
#include <regex.h>
#include <stdio.h>

int main(){


    init();
    char* word = "let x = 5;if (x > 5) then println('not again');end";

    char *p = word;

    while(*p != '\0'){
        int return_len = tokenize(p);

        if (return_len < 0){
            //printf("Unidentified token found... Skipping\n");
            p++;
            continue;
        }
        p += return_len;
    }

    return 0;
}
