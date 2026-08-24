#include "parser.h"
#include "lexer.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include "expressions.h"
#include "statements.h"

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
