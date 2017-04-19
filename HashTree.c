#include <stdlib.h>
#include <string.h>
#include "HashTreeDef.h"
#define TABLE_SIZE2 20
#define INTEGEROFF 2
#define REALOFF 4
#define BOOLEANOFF 1


HashTreeNode* initTree(){
	HashTreeNode *root;
	root= (HashTreeNode*)malloc(1*sizeof(HashTreeNode));
	root->parent=NULL;
	root->tail=0;
	root->curr_offset=0;
	strcpy(root->table_name,"GLOBAL");
	strcpy(root->function_name,"GLOBALF");
	root->table=(HashElement*)malloc(TABLE_SIZE2*sizeof(HashElement));
	int i;
	for(i=0;i<TABLE_SIZE2;i++){
		root->table[i].head=NULL;
	}
	return root;
}

HashTreeNode* addchild2(HashTreeNode *node){
	HashTreeNode *child;
	child= (HashTreeNode*)malloc(1*sizeof(HashTreeNode));
	child->parent=node;
	child->tail=0;
	child->table=(HashElement*)malloc(TABLE_SIZE2*sizeof(HashElement));
	child->curr_offset=0;
	int i;
	for(i=0;i<TABLE_SIZE2;i++){
		child->table[i].head=NULL;
	}
	node->childQ[node->tail]=child;
	node->tail++;
	return child;
}

int hashKey2(char *key){
	int res=0;
	int l=strlen(key);
	int i;
	for(i=l-1;i>=0;i--){
		res+=key[i]-'0';
	}
	return res%TABLE_SIZE2;
}

HashTableNode* find2(char *key,HashTreeNode *node,int res){
	HashElement *table=node->table;
	int hash = hashKey2(key);
	if(table[hash].head==NULL) {
		if(node->parent!=NULL&&res==0) return find2(key,node->parent,0);
		else return NULL;
	}
	else{
		HashTableNode *itr=table[hash].head;
		while(itr!=NULL){
			if(itr->key==NULL) printf("Gadbad hai\n");
			if(strcmp(key,itr->key)==0){ 
				return itr;
			}
			itr=itr->next;
		}
		if(itr==NULL&&node->parent!=NULL&&res==0){
			return find2(key,node->parent,0);
		}
		else{
			return NULL;
		}
	}
}
int addKey2(ASTNode *ast_node,HashTreeNode *node){
	//printf("%s\n",key);
	if(find2(ast_node->tokenptr->lexeme.iden,node,1)!=NULL){
		return -1;
	}
	HashElement *table = (node->table);
	int hash = hashKey2(ast_node->tokenptr->lexeme.iden);
	// int hash=hashKey2(key);
	HashTableNode *tmp = (HashTableNode*)malloc(1*sizeof(HashTableNode));
	// strcpy(tmp->key,key);
	tmp->next=NULL;
	strcpy(tmp->key,ast_node->tokenptr->lexeme.iden);
	tmp->tokenptr=ast_node->tokenptr;
	tmp->ast_node=ast_node;
	tmp->startscope=ast_node->startscope;
	tmp->endscope=ast_node->endscope;
	tmp->datatype=ast_node->dtype;
	int datatemp = tmp->datatype;
	tmp->offset=node->curr_offset;
	int data_incr;
	if (tmp->datatype == 3)
		datatemp = tmp->ast_node->arrtype;
	if(datatemp==0){
		if(tmp->datatype==3)
			data_incr =  (tmp->ast_node->rrange- tmp->ast_node->lrange+1)*INTEGEROFF;
		else
			data_incr= INTEGEROFF;
	}
	else if(datatemp==1){
		if(tmp->datatype==3)
			data_incr =  (tmp->ast_node->rrange- tmp->ast_node->lrange+1)*REALOFF;
		else
			data_incr= REALOFF;
	}
	else if(datatemp==2){
		if(tmp->datatype==3)
			data_incr =  (tmp->ast_node->rrange- tmp->ast_node->lrange+1)*BOOLEANOFF;
		else
			data_incr= BOOLEANOFF;
	}
	node->curr_offset+=data_incr;
	tmp->next=table[hash].head;
	table[hash].head=tmp;
	return 0;
	// printf("%s\n", table[hash].head->key);
}
void add_plist(HashTableNode *ele,ASTNode *node){
	if(node->gnode.non_term!=MODULENT)
	{
		printf("Error in adding i/o lists.\n");
	}
	int i=0;
	ASTNode *iplist=node->child;
	ASTNode *oplist=node->child->sibling;
	while(iplist!=NULL){
		ele->input_plist[i++]=iplist;
		iplist=iplist->sibling;
	}
	i=0;
	while(oplist!=NULL){
		ele->output_plist[i++]=oplist;
		oplist=oplist->sibling;
	}
}