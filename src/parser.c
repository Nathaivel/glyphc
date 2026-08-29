#include "parser.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include "expressions.h"
#include "statements.h"

ASTNode* parse_function_call(TokenArray tokens,int *index){
    ASTNode* function_call = malloc(sizeof(ASTNode));
    function_call->token_type = NODE_FUNCTION_CALL;
    function_call->node.function.identifier = parse_identifier(tokens, index);

    check_token_validity(tokens, index, TOK_LPARAN);
    ASTNode* parameters = program_init(NODE_PARAMETERS);
    (*index)++;

    while (tokens.token_array[*index].token_type != TOK_RPARAN && tokens.token_array[*index].token_type != TOK_EOF){
        //check_token_validity(tokens,index, TOK_IDENTIFIER);
        ASTNode* parameter = parse_logic(tokens, index);
        append_ast_node(&parameters, parameter);

        if (tokens.token_array[*index].token_type != TOK_COMMA){
            break;
        }else{
            (*index)++;
        }
    }
    check_token_validity(tokens,index, TOK_RPARAN);
    (*index)++;

    function_call->node.function.parameters = parameters;

    return function_call;
}

char* str_of_type(TypeKind type){
    switch(type){
        case TYPE_INT:
            return "integer";
            break;
        case TYPE_FLOAT:
            return "float";
            break;
        case TYPE_STRING:
            return  "string";
            break;
        default:
            return "NULL";
            break;
    }
}

TypeKind token_to_type(TokenType token_type){
    switch(token_type){
        case TOK_KEYWORD_INT:
            return TYPE_INT;
            break;
        case TOK_KEYWORD_FLOAT:
            return TYPE_FLOAT;
            break;
        case TOK_KEYWORD_STRING:
            return  TYPE_STRING;
            break;
        default:
            return TYPE_NULL;
            break;
    }
}

ASTNode* program_init(NodeType token_type){
    ASTNode* block = malloc(sizeof(ASTNode));
    block->token_type = token_type;
    block->node.node_list.count = 0;
    block->node.node_list.statements = NULL;

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

ASTNode* create_variable(TokenArray tokens,int *index){
    ASTNode* branch = malloc(sizeof(ASTNode));

    ASTNode* leaf = malloc(sizeof(ASTNode));
    leaf->token_type = NODE_IDENTIFIER;
    leaf->node.identifier = tokens.token_array[*index];


    branch->node.assignment.identifier = leaf;
    branch->node.assignment.expression = NULL;

    (*index)++;
    Token temp = tokens.token_array[*index];



    if (temp.token_type == TOK_COLON){
        branch->token_type = NODE_DECLARATION;

        (*index)++;
        branch->type = token_to_type(tokens.token_array[*index].token_type);
        (*index)++;

    }else{
        branch->token_type = NODE_ASSIGNMENT;
    }

    return branch;
}

ASTNode* create_assignment(TokenArray tokens,int *index){

    ASTNode* branch = create_variable(tokens,index);
    Token temp = tokens.token_array[*index];

    if (temp.token_type == TOK_OPERATOR_ASSIGN){
        (*index)++;
        branch->node.assignment.expression = parse_logic(tokens, index);
        return branch;
    }
    else if (temp.token_type == TOK_OPERATOR_PLUS_EQUALS || temp.token_type == TOK_OPERATOR_MINUS_EQUALS || temp.token_type == TOK_OPERATOR_MULTIPLY_EQUALS || temp.token_type == TOK_OPERATOR_DIVIDE_EQUALS || temp.token_type == TOK_OPERATOR_MODULO_EQUALS){
        (*index)++;
        ASTNode* operator = malloc(sizeof(ASTNode));
        operator->node.binary_op.operation = temp;
        operator->node.binary_op.left = branch->node.assignment.identifier;
        operator->node.binary_op.right = parse_logic(tokens, index);
        branch->node.assignment.expression = operator;
    }

    return branch;
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
        append_ast_node(&block_pointer, statement);
    }
    return block_pointer;
}

void print_statement(ASTNode* program,int depth){
    if (program == NULL) return;

    for (int d = 0;d < depth;d++) printf("  ");

    switch(program->token_type){
        case NODE_PROGRAM:
            printf("PROGRAM\n");
            ASTNode** statements = program->node.node_list.statements;
            for (int i = 0;i < program->node.node_list.count;i++){
                print_statement(statements[i], depth + 1);
            }
            break;
        case NODE_PARAMETERS:
            printf("PARAMETERS\n");
            ASTNode** parameters = program->node.node_list.statements;
            for (int i = 0;i < program->node.node_list.count;i++){
                print_statement(parameters[i], depth + 1);
            }
            break;
        case NODE_ASSIGNMENT:
            printf("(ASSIGNMENT)\n");
            print_statement(program->node.assignment.identifier, depth + 1);
            print_statement(program->node.assignment.expression, depth + 1);
            break;
        case NODE_RETURN:
            printf("(RETURN)\n");
            print_statement(program->node.expression, depth + 1);
            break;
        case NODE_FUNCTION_DECLARATION:
            printf("(FUNCTION DECLARATION)\n");
            print_statement(program->node.function.identifier, depth + 1);
            print_statement(program->node.function.parameters, depth + 1);
            print_statement(program->node.function.block, depth + 1);
            break;
        case NODE_FUNCTION_CALL:
            printf("(FUNCTION CALL)\n");
            print_statement(program->node.function.identifier, depth + 1);
            print_statement(program->node.function.parameters, depth + 1);
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
            printf("(DECLARATION): (%s)\n",str_of_type(program->type));
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

ASTNode* parse(char *p, TokenArray tokens){
    int i = 0;
    ASTNode* tree = malloc(sizeof(ASTNode));
    tree->token_type = NODE_PROGRAM;
    tree->node.node_list.statements = NULL;
    tree->node.node_list.count = 0;

    while(tokens.token_array[i].token_type != TOK_EOF){
        ASTNode* statement = parse_statement(tokens,&i);
        append_ast_node(&tree, statement);
    }
    //printf("%d\n",i);
    print_statement(tree, 0);
    return tree;
}
