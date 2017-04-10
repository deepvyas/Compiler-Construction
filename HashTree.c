#include <stdlib.h>
#include <string.h>
#include "HashTreeDef.h"

#define DTableSize 20
HashTreeNode* initTree(){
	HashTreeNode *root;
	root= (HashTreeNode*)malloc(1*sizeof(HashTreeNode));
	root->parent=NULL;
	root->tail=0;
	root->table=(HashCell*)malloc(20*sizeof(HashCell));
	return root;
}

void addchild(HashTreeNode *node){
	HashTreeNode *child;
	child= (HashTreeNode*)malloc(1*sizeof(HashTreeNode));
	child->parent=node;
	child->tail=0;
	child->table=(HashCell*)malloc(20*sizeof(HashCell));
	node->childQ[node->tail]=child;
	node->tail++;
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

HashNode* find(char *key,HashTreeNode *node){
	HashCell *table=node->table;
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
void addKey(char *key,TokenType token,HashTreeNode *node){
	//printf("%s\n",key);
	HashCell **table = &(node->table);
	int hash = hashKey(key);
	HashNode *tmp = (HashNode*)malloc(sizeof(HashNode));
	strcpy(tmp->key,key);
	tmp->token=token;
	tmp->next=(*table)[hash].head;
	(*table)[hash].head=tmp;
	// printf("%s\n", (*table)[hash].head->key);
}
