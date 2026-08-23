#include "parser.h"
#include "lexer.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

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
            printf("NOT A VALID TOKEN");

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

    printf("%d\n",*index);
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

    printf("%d\n",*index);
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

    printf("%d\n",*index);
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

    printf("%d\n",*index);
    return left;
}

ASTNode* parse_statement(TokenArray tokens,int *index){
    if (tokens.token_array[*index].token_type == TOK_KEYWORD_LET){
        ASTNode* branch = malloc(sizeof(ASTNode));
        branch->token_type = NODE_ASSIGNMENT;

        (*index)++;
        Token temp = tokens.token_array[*index];

        if (temp.token_type == TOK_IDENTIFIER){
            ASTNode* leaf = malloc(sizeof(ASTNode));
            leaf->token_type = NODE_IDENTIFIER;
            leaf->node.identifier = temp;
            branch->node.assignment.identifier = leaf;

            (*index)++;
            Token temp = tokens.token_array[*index];

            if (temp.token_type == TOK_OPERATOR_ASSIGN){
                (*index)++;
                branch->node.assignment.expression = parse_logic(tokens, index);
            }
        }
        return branch;
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
    printf("%d\n",i);
    print_statement(tree, 0);
}
