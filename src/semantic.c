#include "semantic.h"
#include "symbols.h"
#include "parser.h"
#include "token.h"

#include <stdlib.h>
#include <stdio.h>

TypeKind semantic_binary_op_analysis(TypeKind left,TypeKind right, Token operation){
    if (left == TYPE_STRING || right == TYPE_STRING){
        fprintf(stdout,"operation %*.s cannot be used on type string",(int)operation.size,operation.start);
        exit(1);
    }


    if (left == TYPE_INT && right == TYPE_INT && is_arithemeticop(operation.token_type)){
        return TYPE_INT;
    }

    if ((is_numeric(left) && is_numeric(right) && (left == TYPE_FLOAT || right == TYPE_FLOAT)) && is_arithemeticop(operation.token_type)){
        return TYPE_FLOAT;
    }

    if ((is_comparisionalop(operation.token_type) && is_numeric(left) && is_numeric(right)) || (is_logicalop(operation.token_type) && left == TYPE_BOOL && right == TYPE_BOOL)){
        return TYPE_BOOL;
    }


    return TYPE_ERROR;
}

TypeKind lookup_identifier_type(ASTNode* expression, Scope* scope){
    char* identifier_name =  token_value(expression->node.identifier);
    Symbol* identifier = get_symbol_from_table(&scope->table,identifier_name);

    if (identifier == NULL){
        if (scope->parent != NULL){
            return lookup_identifier_type(expression, scope->parent);
        }else{
            fprintf(stdout,"Unkown identifier %s\n",identifier_name);
            exit(1);
        }
    }

    return identifier->type;
}

TypeKind semantic_expression_analysis(ASTNode *expression, Scope* scope){
    if (expression->token_type == NODE_LITERAL){
        return expression->type;
    }

    if (expression->token_type == NODE_IDENTIFIER){
        return lookup_identifier_type(expression,scope);
    }

    if (expression->token_type == NODE_BINARY_OP){
        TypeKind left = semantic_expression_analysis(expression->node.binary_op.left,scope);
        TypeKind right = semantic_expression_analysis(expression->node.binary_op.right,scope);
        return semantic_binary_op_analysis(left,right,expression->node.binary_op.operation);
    }

    return TYPE_VOID;
}

void semantic_assignment_analysis(ASTNode* assignment,Scope* scope){
    ASTNode* identifier = assignment->node.assignment.identifier;
    char* identifier_name = token_value(identifier->node.identifier);


    TypeKind declared_type = lookup_identifier_type(identifier, scope);
    TypeKind derived_type = semantic_expression_analysis(assignment->node.assignment.expression,scope);

    if (declared_type != derived_type){
        fprintf(stdout,"declared type %s does not match the derived type %s\n",str_of_type(declared_type),str_of_type(derived_type));
        exit(1);
    }
}

void semantic_declaration_analysis(ASTNode*  declaration,Scope* scope){
    ASTNode* identifier = declaration->node.assignment.identifier;
    char* identifier_name = token_value(identifier->node.identifier);
    TypeKind declared_type = declaration->type;

    Symbol* variable = get_symbol_from_table(&scope->table, identifier_name);

    if (variable != NULL){
        fprintf(stdout,"Variable redeclaration\n");
        exit(1);
    }

    TypeKind derived_type = semantic_expression_analysis(declaration->node.assignment.expression,scope);

    if (derived_type != declared_type){
        fprintf(stdout,"declared type %s does not match the derived type %s\n",str_of_type(declared_type),str_of_type(derived_type));
        exit(1);
    }

    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->name = identifier_name;
    symbol->type = declared_type;

    add_symbol_to_table(&scope->table, symbol->name, symbol);
}

void semantic_if_statement_analysis(ASTNode* statement,Scope* scope){
    ASTNode* condition = statement->node.if_statement.expression;


    if (condition == NULL){
        fprintf(stdout,"NO condition found quitting...");
        exit(1);
    }

    TypeKind condition_result = semantic_expression_analysis(condition,scope);

    if (condition_result != TYPE_BOOL){
        fprintf(stdout,"condition does not give boolean result quitting...\n");
        exit(1);
    }

    semantic_tree_analysis(statement->node.if_statement.block,scope);

    if (statement->node.if_statement.else_block != NULL) semantic_tree_analysis(statement->node.if_statement.else_block,scope);
}

void semantic_analysis_node(ASTNode* statement,Scope* scope){
    if(statement == NULL){
        fprintf(stdout,"error no statements\n");
        return;
    }

    switch(statement->token_type){
        case (NODE_DECLARATION):
            semantic_declaration_analysis(statement, scope);
            break;

        case (NODE_ASSIGNMENT):
            semantic_assignment_analysis(statement, scope);
            break;
        case (NODE_IF):
            semantic_if_statement_analysis(statement,scope);
            break;
        default:
            break;
    }
}

ASTNode* semantic_tree_analysis(ASTNode* tree,Scope* parent){
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = parent;
    scope->table = init_symbol_table();


    /*
    printf("tree = %p\n",(void*)tree);
    printf("count = %d\n",tree->node.node_list.count);
    printf("statements = %p\n",tree->node.node_list.statements);
    */

    if (tree->token_type == NODE_PROGRAM){
        ASTNode** statements = tree->node.node_list.statements;

        for (int i = 0;i < tree->node.node_list.count;i++){
            ASTNode* statement = statements[i];
            //printf("%d/%d - statement = %p\n",i,tree->node.node_list.count,(void*)statement);
            semantic_analysis_node(statement,scope);
        }
    }

    return tree;
}

ASTNode* semantic_analysis(ASTNode* tree){
    semantic_tree_analysis(tree, NULL);
    return tree;
}
