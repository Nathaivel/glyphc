#include "parser.h"
#include "lexer.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

ASTNode* program_init(){
    ASTNode* block = malloc(sizeof(ASTNode));
    block->token_type = NODE_PROGRAM;
    block->node.program.count = 0;
    block->node.program.statements = NULL;

    return block;
}

ASTNode* create_number_node(char* number,size_t size){
    Token initial;
    initial.token_type = TOK_NUMBER;
    initial.size = size;
    initial.start = number;

    ASTNode* initial_node = malloc(sizeof(ASTNode));
    initial_node->token_type = NODE_LITERAL;
    initial_node->node.literal = initial;

    return initial_node;
}
void check_token_validity(TokenArray tokens, int *index,TokenType target_type){
    if (tokens.token_array[*index].token_type != target_type){
        fprintf(stdout, "expected %s\n",token_type_str(target_type));
        exit(1);
    }
}

void print_statement(ASTNode* program,int depth){
    if (program == NULL) return;

    for (int d = 0;d < depth;d++) printf("  ");

    switch(program->token_type){
        case NODE_PROGRAM:
            printf("PROGRAM\n");
            ASTNode** statements = program->node.program.statements;
            for (int i = 0;i < program->node.program.count;i++){
                print_statement(statements[i], depth + 1);
            }
            break;
        case NODE_ASSIGNMENT:
            printf("(ASSIGNMENT)\n");
            print_statement(program->node.assignment.identifier, depth + 1);
            print_statement(program->node.assignment.expression, depth + 1);
            break;
        case NODE_IF:
            printf("(IF)\n");
            print_statement(program->node.if_statement.expression, depth + 1);
            print_statement(program->node.if_statement.block, depth + 1);
            print_statement(program->node.if_statement.else_block, depth + 1);
            break;
        case NODE_WHILE:
            printf("(WHILE)\n");
            print_statement(program->node.if_statement.expression, depth + 1);
            print_statement(program->node.if_statement.block, depth + 1);
            break;
        case NODE_FOR:
            printf("(FOR)\n");
            print_statement(program->node.for_statement.identifier, depth + 1);
            print_statement(program->node.for_statement.start, depth + 1);
            print_statement(program->node.for_statement.stop, depth + 1);
            print_statement(program->node.for_statement.step, depth + 1);
            print_statement(program->node.for_statement.block, depth + 1);
            break;
        case NODE_DECLARATION:
            printf("(DECLARATION)\n");
            print_statement(program->node.assignment.identifier, depth + 1);
            print_statement(program->node.assignment.expression, depth + 1);
            break;
        case NODE_IDENTIFIER:
            printf("IDENTIFIER: %.*s\n",(int)program->node.identifier.size,program->node.identifier.start);
            break;
        case NODE_LITERAL:
            printf("LITERAL: %.*s\n",(int)program->node.literal.size,program->node.literal.start);
            break;
        case NODE_BINARY_OP:
            printf("OPERATION: %.*s\n",(int)program->node.binary_op.operation.size,program->node.binary_op.operation.start);
            print_statement(program->node.binary_op.left, depth + 1);
            print_statement(program->node.binary_op.right, depth + 1);
            break;
        default:
            printf("NOT A VALID TOKEN %d\n",program->token_type);

    }
}

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

ASTNode* parse_number(TokenArray tokens, int *index){
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->token_type = NODE_LITERAL;
    new_node->node.literal = tokens.token_array[*index];
    (*index)++;
    return new_node;
}

ASTNode* parse_identifier(TokenArray tokens, int *index){
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->token_type = NODE_IDENTIFIER;
    new_node->node.identifier = tokens.token_array[*index];
    (*index)++;
    return new_node;
}

ASTNode* make_binary_op(ASTNode* left,ASTNode* right, Token operator){
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->token_type = NODE_BINARY_OP;
    new_node->node.binary_op.left = left;
    new_node->node.binary_op.right = right;
    new_node->node.binary_op.operation = operator;
    return new_node;
}

ASTNode* parse_factors(TokenArray tokens,int *index){
    Token temp = tokens.token_array[*index];

    if (temp.token_type == TOK_LPARAN){
        (*index)++;
        ASTNode* inner = parse_logic(tokens,index);
        (*index)++;
        return inner;
    }

    if (temp.token_type == TOK_NUMBER){
        return parse_number(tokens, index);
    }else if (temp.token_type == TOK_IDENTIFIER){
        return parse_identifier(tokens, index);
    }else{
        fprintf(stdout,"Syntax error\n");
        exit(1);
    }
}

ASTNode* parse_terms(TokenArray tokens,int *index){
    ASTNode* left = parse_factors(tokens,index);

    while (check_op(&tokens, *index, TOK_OPERATOR_MULTIPLY) || check_op(&tokens, *index, TOK_OPERATOR_DIVIDE) || check_op(&tokens, *index, TOK_OPERATOR_MODULO)){
        Token operator = tokens.token_array[*index];(*index)++;

        ASTNode* right = parse_factors(tokens,index);
        ASTNode* new_node = make_binary_op(left, right,operator);

        left = new_node;
    }

    //printf("%d\n",*index);
    return left;
}

ASTNode* parse_expression(TokenArray tokens,int *index){
    ASTNode* left = parse_terms(tokens,index);

    while (check_op(&tokens, *index, TOK_OPERATOR_MINUS) || check_op(&tokens, *index, TOK_OPERATOR_PLUS)){
        Token operator = tokens.token_array[*index];(*index)++;

        ASTNode* right = parse_terms(tokens,index);
        ASTNode* new_node = make_binary_op(left, right,operator);

        left = new_node;
    }

    //printf("%d\n",*index);
    return left;
}

ASTNode* parse_comparision(TokenArray tokens,int *index){
    ASTNode* left = parse_expression(tokens,index);

    while (check_op(&tokens, *index, TOK_OPERATOR_GREATER) || check_op(&tokens, *index, TOK_OPERATOR_LESSER)|| check_op(&tokens, *index, TOK_OPERATOR_GREATER_EQUALS)|| check_op(&tokens, *index, TOK_OPERATOR_LESSER_EQUALS) || check_op(&tokens, *index, TOK_OPERATOR_EQUALS)){
        Token operator = tokens.token_array[*index];(*index)++;

        ASTNode* right = parse_expression(tokens,index);
        ASTNode* new_node = make_binary_op(left, right,operator);

        left = new_node;
    }

    //printf("%d\n",*index);
    return left;
}

ASTNode* parse_logic(TokenArray tokens,int *index){
    ASTNode* left = parse_comparision(tokens,index);

    while (check_op(&tokens, *index, TOK_OPERATOR_AND) || check_op(&tokens, *index, TOK_OPERATOR_OR)){
        Token operator = tokens.token_array[*index];(*index)++;

        ASTNode* right = parse_comparision(tokens,index);
        ASTNode* new_node = make_binary_op(left, right,operator);

        left = new_node;
    }

    //printf("%d\n",*index);
    return left;
}

ASTNode* create_assignment(TokenArray tokens,int *index,ASTNode* branch){
    ASTNode* leaf = malloc(sizeof(ASTNode));
    leaf->token_type = NODE_IDENTIFIER;
    leaf->node.identifier = tokens.token_array[*index];
    branch->node.assignment.identifier = leaf;

    (*index)++;
    Token temp = tokens.token_array[*index];

    if (temp.token_type == TOK_OPERATOR_ASSIGN){
        (*index)++;
        branch->node.assignment.expression = parse_logic(tokens, index);
        return branch;
    }
    return NULL;
}
ASTNode* create_conditional(TokenArray tokens,int *index,ASTNode* conditional_statement){
    if (tokens.token_array[*index].token_type == TOK_LPARAN){
        conditional_statement->node.if_statement.expression = parse_logic(tokens, index);
    }else{
        fprintf(stdout, "no ( braces for condition\n");
        exit(1);
    }

    return conditional_statement;
}

ASTNode* create_block(TokenArray tokens, int *index,ASTNode* block_pointer){
    while(tokens.token_array[*index].token_type != TOK_EOF && tokens.token_array[*index].token_type != TOK_KEYWORD_END && tokens.token_array[*index].token_type != TOK_KEYWORD_ELSE){
        ASTNode* statement = parse_statement(tokens,index);
        append_statement(&block_pointer, statement);
    }
    return block_pointer;
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

void parse(char *p){
    TokenArray tokens = lex(p);
    int i = 0;
    ASTNode* tree = malloc(sizeof(ASTNode));
    tree->token_type = NODE_PROGRAM;
    tree->node.program.statements = NULL;
    tree->node.program.count = 0;

    while(tokens.token_array[i].token_type != TOK_EOF){
        ASTNode* statement = parse_statement(tokens,&i);
        append_statement(&tree, statement);
    }
    //printf("%d\n",i);
    print_statement(tree, 0);
}
