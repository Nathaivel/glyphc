#include "parser.h"
#include "expressions.h"
#include <stdlib.h>
#include <stdio.h>

void append_statement(ASTNode** program, ASTNode* statement){
    if ((*program)->node.program.statements == NULL){
        (*program)->node.program.capacity = 8;
        (*program)->node.program.count = 0;
        (*program)->node.program.statements = malloc(sizeof(ASTNode*)*(*program)->node.program.capacity);
        //printf("Success");
    }else if ((*program)->node.program.count >= (*program)->node.program.capacity){
        (*program)->node.program.capacity *= 2;
        ASTNode **tmp = realloc((*program)->node.program.statements,sizeof(ASTNode*)*(*program)->node.program.capacity);

        if (tmp == NULL){
            fprintf(stderr, "realloc failed\n");
            exit(1);
        }

        (*program)->node.program.statements = tmp;
    }

    (*program)->node.program.statements[(*program)->node.program.count] = statement;
    (*program)->node.program.count += 1;
}


ASTNode* parse_for_statement(TokenArray tokens,int *index){
    ASTNode* for_statement = malloc(sizeof(ASTNode));
    for_statement->token_type = NODE_FOR;

    for_statement->node.for_statement.start = create_number_node("0", 1);
    for_statement->node.for_statement.step = create_number_node("1", 1);

    check_token_validity(tokens, index, TOK_IDENTIFIER);


    for_statement->node.for_statement.identifier = parse_identifier(tokens, index);


    check_token_validity(tokens, index, TOK_NUMBER);
    for_statement->node.for_statement.stop = parse_number(tokens, index);

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_TO){
        (*index)++;
        check_token_validity(tokens, index, TOK_NUMBER);
        for_statement->node.for_statement.start = for_statement->node.for_statement.stop;
        for_statement->node.for_statement.stop = parse_number(tokens, index);
    }

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_BY){
        (*index)++;
        check_token_validity(tokens, index, TOK_NUMBER);
        for_statement->node.for_statement.step = parse_number(tokens, index);
    }



    check_token_validity(tokens, index, TOK_KEYWORD_THEN);

    ASTNode* for_block = program_init();
    (*index)++;
    for_statement->node.for_statement.block = create_block(tokens, index, for_block);

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_END){
        (*index)++;
        return for_statement;
    }
    else {
        fprintf(stdout,"Expected 'end'");
        exit(1);
    }
    return NULL;
}

ASTNode* parse_while_statement(TokenArray tokens,int *index){
    ASTNode* while_statement = malloc(sizeof(ASTNode));
    while_statement->token_type = NODE_WHILE;
    while_statement->node.if_statement.else_block = NULL;
    create_conditional(tokens, index, while_statement);

    if (tokens.token_array[*index].token_type != TOK_KEYWORD_THEN){
        fprintf(stdout, "expected 'then'\n");
        exit(1);
    }

    ASTNode* while_block = program_init();
    (*index)++;
    while_statement->node.if_statement.block = create_block(tokens, index, while_block);

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_END){
        (*index)++;
        return while_statement;
    }
    else {
        fprintf(stdout,"Expected 'end'");
        exit(1);
    }
    return NULL;
}

ASTNode* parse_if_statement(TokenArray tokens,int *index){
    ASTNode* if_statement = malloc(sizeof(ASTNode));
    if_statement->token_type = NODE_IF;
    if_statement->node.if_statement.else_block = NULL;

    if_statement = create_conditional(tokens, index, if_statement);

    check_token_validity(tokens, index, TOK_KEYWORD_THEN);



    ASTNode* if_block = program_init();
    ASTNode* else_block = NULL;

    (*index)++;
    if_statement->node.if_statement.block = create_block(tokens, index, if_block);

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_ELSE){
        (*index)++;
        else_block = program_init();
        if_statement->node.if_statement.else_block = create_block(tokens, index, else_block);
    }

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_END){
        (*index)++;
        return if_statement;
    }
    else {
        fprintf(stdout,"Expected 'end'");
        exit(1);
    }
    return NULL;
}

ASTNode* parse_statement(TokenArray tokens,int *index){

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_IF){
        (*index)++;
        return parse_if_statement(tokens,index);
    }

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_WHILE){
        (*index)++;
        return parse_while_statement(tokens,index);
    }

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_FOR){
        (*index)++;
        return parse_for_statement(tokens,index);
    }

    if (tokens.token_array[*index].token_type == TOK_IDENTIFIER){
        ASTNode* branch = malloc(sizeof(ASTNode));
        branch->token_type = NODE_ASSIGNMENT;
        return create_assignment(tokens,index,branch);
    }



    if (tokens.token_array[*index].token_type == TOK_KEYWORD_LET){
        ASTNode* branch = malloc(sizeof(ASTNode));
        branch->token_type = NODE_DECLARATION;

        (*index)++;
        Token temp = tokens.token_array[*index];

        if (temp.token_type == TOK_IDENTIFIER){
            return create_assignment(tokens,index,branch);
        }
    }


    (*index)++;
    return NULL;
}
