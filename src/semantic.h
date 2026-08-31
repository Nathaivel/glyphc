#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include "symbols.h"


ASTNode* semantic_tree_analysis(ASTNode* tree,Scope* parent);
ASTNode* semantic_analysis(ASTNode* tree);
TypeKind semantic_binary_op_analysis(TypeKind left,TypeKind right, Token operation);
Scope* create_scope(Scope* parent);
TypeKind lookup_identifier_type(ASTNode* expression, Scope* scope);
TypeKind semantic_expression_analysis(ASTNode *expression, Scope* scope);
void semantic_assignment_analysis(ASTNode* assignment,Scope* scope);
Symbol* create_symbol(char* identifier_name,TypeKind declared_type,SymbolKind symbol_kind);
Symbol* create_symbol_from_ast(ASTNode* identifier,TypeKind type,ASTNode* expression,SymbolKind symbol_kind,Scope* scope);
void semantic_declaration_analysis(ASTNode*  declaration,Scope* scope);
Symbol** get_parameter_symbols(ASTNode* parameter_body,Scope* scope);
void semantic_function_declaration_statement_analysis(ASTNode* statement,Scope* scope);
void semantic_return_statement_analysis(ASTNode* statement,Scope* scope);
TypeKind semantic_function_call_analysis(ASTNode* statement,Scope* scope);
void semantic_conditional_statement_analysis(ASTNode* statement,Scope* scope);
void semantic_analysis_node(ASTNode* statement,Scope* scope);
ASTNode* semantic_tree_analysis(ASTNode* tree,Scope* scope);
ASTNode* semantic_analysis(ASTNode* tree);

#endif
