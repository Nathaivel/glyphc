#include "parser.h"
#include "expressions.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

void append_ast_node(ASTNode** program, ASTNode* statement){
    if ((*program)->node.node_list.statements == NULL){
        (*program)->node.node_list.capacity = 8;
        (*program)->node.node_list.count = 0;
        (*program)->node.node_list.statements = malloc(sizeof(ASTNode*)*(*program)->node.node_list.capacity);
        //printf("Success");
    }else if ((*program)->node.node_list.count >= (*program)->node.node_list.capacity){
        (*program)->node.node_list.capacity *= 2;
        ASTNode **tmp = realloc((*program)->node.node_list.statements,sizeof(ASTNode*)*(*program)->node.node_list.capacity);

        if (tmp == NULL){
            fprintf(stderr, "realloc failed\n");
            exit(1);
        }

        (*program)->node.node_list.statements = tmp;
    }

    (*program)->node.node_list.statements[(*program)->node.node_list.count] = statement;
    (*program)->node.node_list.count += 1;
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

    ASTNode* for_block = program_init(NODE_PROGRAM);
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

    ASTNode* while_block = program_init(NODE_PROGRAM);
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



    ASTNode* if_block = program_init(NODE_PROGRAM);
    ASTNode* else_block = NULL;

    (*index)++;
    if_statement->node.if_statement.block = create_block(tokens, index, if_block);

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_ELSE){
        (*index)++;
        else_block = program_init(NODE_PROGRAM);
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

ASTNode* parse_function(TokenArray tokens,int *index){
    ASTNode* function = malloc(sizeof(ASTNode));
    function->token_type = NODE_FUNCTION_DECLARATION;

    check_token_validity(tokens,index, TOK_IDENTIFIER);
    function->node.function.identifier = parse_identifier(tokens, index);

    check_token_validity(tokens,index, TOK_LPARAN);
    (*index)++;

    ASTNode* parameters = program_init(NODE_PARAMETERS);

    while (tokens.token_array[*index].token_type != TOK_RPARAN && tokens.token_array[*index].token_type != TOK_EOF){
         check_token_validity(tokens,index, TOK_IDENTIFIER);
         ASTNode* parameter = parse_identifier(tokens, index);
         append_ast_node(&parameters, parameter);

         if (tokens.token_array[*index].token_type != TOK_COMMA){
             break;
         }else{
             (*index)++;
         }
    }
    check_token_validity(tokens,index, TOK_RPARAN);
    (*index)++;
    function->node.function.parameters = parameters;

    ASTNode* function_block = program_init(NODE_PROGRAM);

    check_token_validity(tokens,index, TOK_KEYWORD_THEN);
    (*index)++;

    create_block(tokens,index, function_block);

    check_token_validity(tokens,index, TOK_KEYWORD_END);
    (*index)++;

    function->node.function.block = function_block;

    return function;
}




ASTNode* parse_statement(TokenArray tokens,int *index){
    if (tokens.token_array[*index].token_type == TOK_KEYWORD_FUNCTION){
        (*index)++;
        return parse_function(tokens,index);
    }

    if (tokens.token_array[*index].token_type == TOK_KEYWORD_RETURN){
        (*index)++;
        ASTNode* return_statement = malloc(sizeof(ASTNode));
        return_statement->token_type = NODE_RETURN;
        return_statement->node.expression = parse_logic(tokens, index);
        return return_statement;
    }
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
        if (tokens.token_array[*index+1].token_type == TOK_LPARAN) return parse_function_call(tokens,index);
        else return create_assignment(tokens,index);
    }


/*
    if (tokens.token_array[*index].token_type == TOK_KEYWORD_LET){
        ASTNode* branch = malloc(sizeof(ASTNode));
        branch->token_type = NODE_DECLARATION;

        (*index)++;
        Token temp = tokens.token_array[*index];

        if (temp.token_type == TOK_IDENTIFIER){
            return create_assignment(tokens,index,branch);
        }
    }
*/

    (*index)++;
    return NULL;
}
