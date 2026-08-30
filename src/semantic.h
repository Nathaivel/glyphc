#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include "symbols.h"


ASTNode* semantic_tree_analysis(ASTNode* tree,Scope* parent);
ASTNode* semantic_analysis(ASTNode* tree);


#endif
