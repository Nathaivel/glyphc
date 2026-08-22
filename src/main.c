#include "lexer.h"
#include "token.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

char* read_entire_file(const char *path,size_t *out_len){
    FILE* file = fopen(path, "r");
    if (file == NULL){
        perror("File not found quitting..\n");
        exit(1);
    }
    fseek(file,0,SEEK_END);
    long size = ftell(file);
    fseek(file,0,SEEK_SET);

    char* buffer = malloc(size+1);
    if (!buffer){ fprintf(stderr,"Out of memory\n");exit(1); }

    fread(buffer,1,size,file);
    buffer[size] = '\0';
    fclose(file);

    if (out_len) *out_len = size;
    return buffer;
}
int main(int argc,char **argv){

    if (argc <= 1){
        printf("No file specified quitting\n");
    }

    init();
    TokenArray tokens;
    array_init(&tokens);

    //char* word = "let x = 5;if (x > 5) then println('not again');end";
    size_t out_len;
    char* p = read_entire_file(argv[1],&out_len);

    while(*p != '\0'){
            int return_len = tokenize(p,&tokens);

            if (return_len < 0){
                //printf("Unidentified token found... Skipping\n");
                p++;
                continue;
            }
            p += return_len;
        }

    print_array(&tokens);

    return 0;
}
