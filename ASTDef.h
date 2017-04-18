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
	valueAST value;
	//==
	//ID or Module name==
	int dtype;  // 0 integer, 1 real, 2 bool, 3 array
	int lrange;
	int rrange;
	int arrtype; // 0 integer arr, 1 real arr, 2 bool arr
	//SymbolTable Ptr;
	int scope;
	int startscope;
	int endscope;
	tokenInfo *tokenptr;
	//==
	//For operators
	// STMT TYPE
	int stmttype;
	//if IOSTMT
	//VAR
	int vartype;
	int sign;
	// ITERTIVESMT
	int looptype;
	//IDLIST OR STATEMENT LIST 
	int listcount;
	struct ASTNode *lop,*rop;
	//Family
	struct ASTNode *parent,*child,*sibling;
};

typedef struct ASTNode ASTNode;

/*Function prototypes*/
ASTNode* genAST(ParseTreeNode *proot,ASTNode *parent);
ASTNode* compressList(ParseTreeNode *proot,ASTNode *parent);
#endif
