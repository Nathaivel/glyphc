#include "parser.h"
#include "token.h"
#include "expressions.h"
#include <stdlib.h>
#include <stdio.h>


TypeKind get_literal_type(Token token){
    switch (token.token_type){
        case TOK_STRING:
            return TYPE_STRING;
            break;
        case TOK_INTEGER:
            return TYPE_INT;
            break;
        case TOK_FLOAT:
            return TYPE_FLOAT;
            break;
        case TOK_KEYWORD_FALSE:
            return TYPE_BOOL;
            break;
        case TOK_KEYWORD_TRUE:
            return TYPE_BOOL;
            break;
        default:
            return TYPE_VOID;
    }
}

ASTNode* parse_literal(TokenArray tokens, int *index){
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->token_type = NODE_LITERAL;
    new_node->source = &tokens.token_array[*index];
    new_node->type = get_literal_type(tokens.token_array[*index]);
    new_node->node.literal = tokens.token_array[*index];
    (*index)++;
    return new_node;
}

ASTNode* parse_identifier(TokenArray tokens, int *index){
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->token_type = NODE_IDENTIFIER;
    new_node->source = &tokens.token_array[*index];
    new_node->node.identifier = tokens.token_array[*index];
    (*index)++;
    return new_node;
}



ASTNode* make_binary_op(ASTNode* left,ASTNode* right, Token operation){
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->token_type = NODE_BINARY_OP;

    new_node->node.binary_op.left = left;
    new_node->node.binary_op.right = right;
    new_node->node.binary_op.operation = operation;
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

    if (temp.token_type == TOK_INTEGER || temp.token_type == TOK_FLOAT || temp.token_type == TOK_STRING || temp.token_type == TOK_KEYWORD_FALSE || temp.token_type == TOK_KEYWORD_TRUE){
        return parse_literal(tokens, index);
    }else if (temp.token_type == TOK_IDENTIFIER){
        if (tokens.token_array[*index+1].token_type == TOK_LPARAN) return parse_function_call(tokens,index);
        return parse_identifier(tokens, index);
    }else{
        char* error = "Syntax error";
        char error_msg[64];

        snprintf(error_msg, sizeof(error_msg), "Expected expression found %s", token_type_str(tokens.token_array[*index].token_type));
        syntax_error(temp,
            tokens.name,
            tokens.source,
            error,
            error_msg);
        return NULL;
    }
}

ASTNode* parse_terms(TokenArray tokens,int *index){
    ASTNode* left = parse_factors(tokens,index);

    while (check_op(&tokens, *index, TOK_OPERATOR_MULTIPLY) || check_op(&tokens, *index, TOK_OPERATOR_DIVIDE) || check_op(&tokens, *index, TOK_OPERATOR_MODULO)){
        Token operator = tokens.token_array[*index];(*index)++;

        ASTNode* right = parse_factors(tokens,index);
        ASTNode* new_node = make_binary_op(left, right,operator);
        new_node->source = &tokens.token_array[*index];

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
        new_node->source = &tokens.token_array[*index];

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
        new_node->source = &tokens.token_array[*index];

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
        new_node->source = &tokens.token_array[*index];

        left = new_node;
    }

    //printf("%d\n",*index);
    return left;
}
