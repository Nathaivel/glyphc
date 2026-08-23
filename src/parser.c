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
ASTNode* parse_expression(TokenArray tokens,int *index){
    ASTNode* left = parse_number(tokens,index);

    while (tokens.token_array[*index].token_type != TOK_NEWLINE && tokens.token_array[*index].token_type != TOK_EOF && is_binaryop(tokens.token_array[*index].token_type)){
        Token operator = tokens.token_array[*index];
        (*index)++;

        ASTNode* right = parse_number(tokens,index);
        ASTNode* new_node = malloc(sizeof(ASTNode));
        new_node->token_type = NODE_BINARY_OP;
        new_node->node.binary_op.left = left;
        new_node->node.binary_op.right = right;
        new_node->node.binary_op.operation = operator;
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
                branch->node.assignment.expression = parse_expression(tokens, index);
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
