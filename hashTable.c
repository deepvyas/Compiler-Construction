#include "hashTable.h"
#include <stdlib.h>
#include <string.h>

HashCell *HashTable;

void initTable(){
    HashCell *table;
    table = (HashCell*)malloc(TABLE_SIZE*sizeof(HashCell));
    int i =0;
    for(i = 0; i < TABLE_SIZE; i++){
        table[i].head = NULL;
    }
    populate(table);
    HashTable = table;
}

int hashKey(char *key){
	int res=0;
	int l=strlen(key);
	int i;
	for(i=l-1;i>=0;i--){
		res+=key[i]-'0';
	}
	return res%TABLE_SIZE;
}

HashNode* find(char *key,HashCell* table){
	int hash = hashKey(key);
	if(table[hash].head==NULL) return NULL;
	else{
		HashNode *itr=table[hash].head;
		while(itr!=NULL){
			if(strcmp(key,itr->key)==0) return itr;
			itr=itr->next;
		}
		return NULL;
	}
}

void addKey(char *key,TokenType token,HashCell **table){
	//printf("%s\n",key);
	int hash = hashKey(key);
	HashNode *tmp = (HashNode*)malloc(sizeof(HashNode));
	strcpy(tmp->key,key);
	tmp->token=token;
	tmp->next=(*table)[hash].head;
	(*table)[hash].head=tmp;
	// printf("%s\n", (*table)[hash].head->key);
}

void populate(HashCell *table){
	addKey("integer",INTEGER, &table);
	addKey("real",REAL,&table);
	addKey("boolean",BOOLEAN, &table);
	addKey("of",OF,&table);
	addKey("array",ARRAY,&table);
	addKey("start",START,&table);
	addKey("end",END,&table);
	addKey("declare",DECLARE,&table);
	addKey("module",MODULE,&table);
	addKey("driver",DRIVER,&table);
	addKey("program",PROGRAM,&table);
	addKey("get_value",GET_VALUE,&table);
	addKey("print",PRINT,&table);
	addKey("use",USE,&table);
	addKey("with",WITH,&table);
	addKey("parameters",PARAMETERS,&table);
	addKey("true",TRUE,&table);
	addKey("false",FALSE,&table);
	addKey("takes",TAKES,&table);
	addKey("input",INPUT,&table);
	addKey("returns",RETURNS,&table);
	addKey("AND",AND,&table);
	addKey("OR",OR,&table);
	addKey("for",FOR,&table);
	addKey("in",IN,&table);
	addKey("switch",SWITCH,&table);
	addKey("case",CASE,&table);
	addKey("break",BREAK,&table);
	addKey("default",DEFAULT,&table);
	addKey("while",WHILE,&table);
}
