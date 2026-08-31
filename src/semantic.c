#include "semantic.h"
#include "symbols.h"
#include "parser.h"
#include "token.h"

#include <stdlib.h>
#include <stdio.h>

Scope* create_scope(Scope* parent){
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = parent;
    scope->table = init_symbol_table();

    return scope;
}

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

    if (expression->token_type == NODE_FUNCTION_CALL){
        return semantic_function_call_analysis(expression,scope);
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

Symbol* create_symbol(char* identifier_name,TypeKind declared_type,SymbolKind symbol_kind){
    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->name = identifier_name;
    symbol->type = declared_type;
    symbol->symbol_type = symbol_kind;

    return symbol;
}

Symbol* create_symbol_from_ast(ASTNode* identifier,TypeKind type,ASTNode* expression,SymbolKind symbol_kind,Scope* scope){
    char* identifier_name = token_value(identifier->node.identifier);
    TypeKind declared_type = type;


    if (scope != NULL){
        Symbol* variable = get_symbol_from_table(&scope->table, identifier_name);
        if (variable != NULL){
            fprintf(stdout,"Variable redeclaration\n");
            exit(1);
        }

        if (expression != NULL){
            TypeKind derived_type = semantic_expression_analysis(expression,scope);

            if (derived_type != declared_type){
                fprintf(stdout,"declared type %s does not match the derived type %s\n",str_of_type(declared_type),str_of_type(derived_type));
                exit(1);
            }
        }
    }

    Symbol* symbol = create_symbol(identifier_name, declared_type, symbol_kind);
    return symbol;
}

void semantic_declaration_analysis(ASTNode*  declaration,Scope* scope){
    ASTNode* identifier = declaration->node.assignment.identifier;
    Symbol* symbol = create_symbol_from_ast(
        identifier,
        declaration->type,
        declaration->node.assignment.expression,
        SYMBOL_VARIABLE,
        scope
    );

    add_symbol_to_table(&scope->table, symbol->name, symbol);
}

Symbol** get_parameter_symbols(ASTNode* parameter_body,Scope* scope){
    Symbol** parameter_symbols = calloc(parameter_body->node.node_list.count,sizeof(Symbol*));

    ASTNode** parameters = parameter_body->node.node_list.statements;
    for (int i = 0;i < parameter_body->node.node_list.count;i++){
        ASTNode* parameter = parameters[i];
        parameter_symbols[i] = create_symbol_from_ast(parameter->node.assignment.identifier, parameter->type, NULL, SYMBOL_PARAMETER, scope);

        add_symbol_to_table(&scope->table, parameter_symbols[i]->name,parameter_symbols[i]);
        printf("%s: %s\n",parameter_symbols[i]->name,str_of_type(parameter_symbols[i]->type));
    }

    return parameter_symbols;
}

void semantic_function_declaration_statement_analysis(ASTNode* statement,Scope* scope){
    ASTNode* identifier = statement->node.function.identifier;
    char* identifier_name = token_value(identifier->node.identifier);

    Symbol* function = create_symbol_from_ast(identifier,TYPE_VOID, NULL, SYMBOL_FUNCTION, scope);
    Scope* function_scope = create_scope(scope);

    Symbol** parameters = get_parameter_symbols(statement->node.function.parameters,function_scope);

    function->parameters = parameters;
    function->parameter_count = statement->node.function.parameters->node.node_list.count;
    function_scope->is_returnable = true;

    semantic_tree_analysis(statement->node.function.block,function_scope);


    if (function_scope->has_return){
        function->type = function_scope->return_type;
    }

    add_symbol_to_table(&scope->table, identifier_name, function);
}

void semantic_return_statement_analysis(ASTNode* statement,Scope* scope){
    TypeKind return_expression = semantic_expression_analysis(statement->node.expression, scope);
    Scope* target_scope = scope;

    while (target_scope->parent != NULL && !target_scope->is_returnable){
            target_scope = target_scope->parent;
    }

    if (target_scope == NULL){
        fprintf(stdout,"error: return used ouside function body\n");
        exit(1);
    }

    if (target_scope->has_return){
        if (target_scope->return_type != return_expression){
            fprintf(stdout,"error: multiple return types encountered %s and %s\n",str_of_type(target_scope->return_type),str_of_type(return_expression));
            exit(1);
        }
    }

    target_scope->has_return = true;
    target_scope->return_type = return_expression;
}

TypeKind semantic_function_call_analysis(ASTNode* statement,Scope* scope){
    ASTNode* identifier = statement->node.function.identifier;
    char* identifier_name = token_value(identifier->node.identifier);

    Symbol* function = get_symbol_from_table(&scope->table, identifier_name);

    if (function == NULL){
        fprintf(stdout, "The function %s is not found in the current scope",identifier_name);
        exit(1);
    }

    int arguement_count = statement->node.function.parameters->node.node_list.count;
    if (function->parameter_count != arguement_count){
        fprintf(stdout, "The expected %d arguements got %d arguements",function->parameter_count,arguement_count);
        exit(1);
    }

    ASTNode** arguements = statement->node.function.parameters->node.node_list.statements;

    for(int i = 0;i < arguement_count;i++){
        TypeKind result_type = semantic_expression_analysis(arguements[i],scope);

        if (result_type != function->parameters[i]->type){
            fprintf(stdout,"Expected type %s for arguement %s got type %s\n",str_of_type(function->parameters[i]->type),function->parameters[i]->name,str_of_type(result_type));
            exit(1);
        }
    }

    return function->type;

}

void semantic_conditional_statement_analysis(ASTNode* statement,Scope* scope){
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

    semantic_tree_analysis(statement->node.if_statement.block,create_scope(scope));

    if (statement->node.if_statement.else_block != NULL) semantic_tree_analysis(statement->node.if_statement.else_block,create_scope(scope));
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
            semantic_conditional_statement_analysis(statement,scope);
            break;
        case (NODE_WHILE):
            semantic_conditional_statement_analysis(statement,scope);
            break;
        case (NODE_FUNCTION_DECLARATION):
            semantic_function_declaration_statement_analysis(statement,scope);
            break;
        case (NODE_RETURN):
            semantic_return_statement_analysis(statement, scope);
            break;
        case (NODE_FUNCTION_CALL):
            semantic_function_call_analysis(statement, scope);
            break;
        default:
            break;
    }
}

ASTNode* semantic_tree_analysis(ASTNode* tree,Scope* scope){
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
    semantic_tree_analysis(tree, create_scope(NULL));
    return tree;
}
