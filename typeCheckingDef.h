#ifndef _TYPECHECKING_
#define _TYPECHECKING_

#include "ASTDef.h"
#include "HashTreeDef.h"

void testAST(ASTNode *root);
int evaluateOperator(ASTNode *node, char ch);
int parseAST(ASTNode *root, HashTreeNode *htroot);
int checkType(ASTNode *inpChild, ASTNode* inHT, HashTreeNode *htroot);
int parseASTAgain(ASTNode *root, HashTreeNode *globalHT);

#endif
