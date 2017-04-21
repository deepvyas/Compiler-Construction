// Batch Number - 38
// 2014A7PS102P - RISHABH JOSHI
// 2014A7PS248P - DEEP VYAS


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
