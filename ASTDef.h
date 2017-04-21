#ifndef _ASTDEF_
#define _ASTDEF_

#include <stdbool.h>
#include "parserDef.h"
#include "lexerDef.h"
#include "hashTable.h"
//#include "HashTreeDef.h"
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
	//SymbolTable Ptr
	int startscope;
	int endscope;
	tokenInfo *tokenptr;
    int scope;
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
	/*FOR CODE GEN*/
	int decode;
	struct ASTNode *lop,*rop;
    // Pointer to the hashTable made void* and typecasted later
    void *htPointer;

    // FOR CODE GENERATION
    int memoryLocation;
	//Family
	struct ASTNode *parent,*child,*sibling;
};

typedef struct ASTNode ASTNode;

/*Function prototypes*/
ASTNode* genAST(ParseTreeNode *proot,ASTNode *parent);
ASTNode* compressList(ParseTreeNode *proot,ASTNode *parent);
ASTNode* create_ast_node();
int extract_type(ParseTreeNode *node);
ASTNode* make_id_node(ParseTreeNode *idnode,ParseTreeNode *dnode);
ASTNode* compressList(ParseTreeNode *proot,ASTNode *parent);
ASTNode* resolve_var(ParseTreeNode *node,ASTNode *parent);
ASTNode* resolve_assgn_stmt(ParseTreeNode *node,ASTNode *parent);
ASTNode* resolve_module_stmt(ParseTreeNode *node,ASTNode *parent);
ASTNode* resolve_exp_recursive(ParseTreeNode *node,ASTNode *parent);
ASTNode* genAST(ParseTreeNode *proot,ASTNode *parent);
void _printAST(ASTNode *ast_root);
ASTNode* makeAST(char *filename);
#endif
