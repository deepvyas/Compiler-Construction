// Batch Number - 38
// 2014A7PS102P - RISHABH JOSHI
// 2014A7PS248P - DEEP VYAS

/*Lexeme Size defined in lexerDef*/
#ifndef _HASHTABLE_
#define _HASHTABLE_
#define TABLE_SIZE 32
#include "lexerDef.h"
#include <stdlib.h>
#include <string.h>

struct HashNode{
	char key[LEXEME_SIZE];
	TokenType token;
	struct HashNode *next;
};

typedef struct HashNode HashNode;

typedef enum{
	FREE,FULL,DELETED
}CellStatus;

typedef struct{
	HashNode *head;
}HashCell;

// HashCell *HashTable;

void initTable();
int hashKey(char *key);
HashNode* find(char *key,HashCell* table);
void addKey(char *key,TokenType token,HashCell **table);
void populate(HashCell *table);

#endif
