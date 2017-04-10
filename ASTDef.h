#ifndef _ASTDEF_
#define _ASTDEF_

#include <stdbool.h>
#include "./Stage1/parserDef.h"
#include "./Stage1/lexerDef.h"
#include "Stage1/hashTable.h"
typedef union{
	int num;
	float r_num;
	int tval;
}valueAST;

struct ASTNode{
	//Arithmetic or Boolean Nodes non-terminals==
	GNodeType gnode;
	tag t;
	valueAST ast;
	//==
	//ID or Module name==
	int dtype;
	int lrange;
	int rrange;
	int arrtype;
	//SymbolTable Ptr;
	int scope;
	tokenInfo *tokenptr;
	//==
	//For operators
	struct ASTNode *lop,*rop;
	//Family
	struct ASTNode *parent,*child,*sibling;
};

typedef struct ASTNode ASTNode;

/*Function prototypes*/
ASTNode* genAST(ParseTreeNode *proot,ASTNode *parent);
ASTNode* compressList(ParseTreeNode *proot,ASTNode *parent);
#endif
