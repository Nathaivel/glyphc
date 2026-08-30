#include "parser.h"
#include "lexer.h"
#include "semantic.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    //char* word = "let x = 5;if (x > 5) then println('not again');end";


    size_t out_len;
    char* p = read_entire_file(argv[argc - 1],&out_len);
    TokenArray tokens = lex(p);
    tokens.name = argv[argc - 1];
    ASTNode* ast = parse(p,tokens);
    SymbolHashMap table = init_symbol_table();
    Symbol value_1 = {"hello",TYPE_STRING};
    add_symbol_to_table(&table, "hello", &value_1);


    printf("{key: %s,type: %s}\n",get_symbol_from_table(&table, "hello")->name,str_of_type(get_symbol_from_table(&table, "hello")->type));



    return 0;
}
