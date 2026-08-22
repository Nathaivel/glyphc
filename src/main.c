#include "lexer.h"
#include <regex.h>
#include <stdio.h>

int main(int argc,char **argv){

    if (argc <= 1){
        printf("No file specified quitting\n");
    }

    init();
    //char* word = "let x = 5;if (x > 5) then println('not again');end";
    char input[250];
    FILE* test_file = fopen(argv[1], "r");

    if (test_file == NULL){
        printf("File not found quitting..\n");
        return 1;
    }

    while (fgets(input, sizeof(input),test_file) != NULL){
        char *p = input;

        while(*p != '\0'){
            int return_len = tokenize(p);

            if (return_len < 0){
                //printf("Unidentified token found... Skipping\n");
                p++;
                continue;
            }
            p += return_len;
        }
    }

    return 0;
}
