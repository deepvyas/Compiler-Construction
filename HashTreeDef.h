#ifndef _ASTDEF_
#define _ASTDEF_
#include "Stage1/lexerDef.h"
#include "./Stage1/parserDef.h"
#include "./Stage1/lexerDef.h"
#define NOC 20
struct HashNode{
	char key[LEXEME_SIZE];
	TokenType token;
	int datatype;
	struct HashNode *next;
};

typedef struct HashNode HashNode;

typedef struct{
	HashNode *head;
}HashCell;

struct HashTreeNode{
	HashCell *table;
	struct HashTreeNode* childQ[NOC];
	int tail;
	struct HashTreeNode *parent;
};

typedef struct HashTreeNode HashTreeNode;
#endif