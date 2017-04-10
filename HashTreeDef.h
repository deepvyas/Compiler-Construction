#ifndef _ASTDEF_
#define _ASTDEF_

#define NOC 20
struct HashNode{
	char key[LEXEME_SIZE];
	TokenType token;
	struct HashNode *next;
};

typedef struct HashNode HashNode;

typedef struct{
	HashNode *head;
}HashCell;

struct HashTreeNode{
	HashCell *table;
	struct HashTreeNode* childQ[NOC];
	int tail=0;
	struct HashTreeNode *parent;
};

typedef struct HashTreeNode HashTreeNode;
#endif