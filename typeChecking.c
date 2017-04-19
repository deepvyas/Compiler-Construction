#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"
#include "HashTreeDef.h"

/*To Be Removed*/
void testAST(ASTNode *root){
	if(root->t==NONTERMINAL&&(root->gnode.non_term==MODULEDECLARATION||
		root->gnode.non_term==DRIVERMODULE||
		root->gnode.non_term==MODULENT||
		root->gnode.non_term==MODULEREUSESTMT)){
		printf("Module : %s\n",root->tokenptr->lexeme.iden);
	}
	if(root->t==TERMINAL&&root->gnode.term==ID&&root->parent->gnode.non_term!=VAR){
		printf("ID : %s with type : %d",root->tokenptr->lexeme.iden,root->dtype);
		if(root->parent->stmttype==DECLARESTMT||
			root->parent->gnode.non_term==INPUTPLIST||
			root->parent->gnode.non_term==OUTPUTPLIST){
			printf("(ID Declared Here.)");
		}
		if(root->dtype==3){
			printf(" and lrange :%d ,rrange :%d\n",root->lrange,root->rrange);
		}
		else printf("\n");

	}
	else if(root->t==NONTERMINAL&&root->gnode.non_term==VAR){
		if(root->vartype==1){
			printf("ID : %s from VAR\n",root->tokenptr->lexeme.iden);
		}
		else if(root->vartype==2){
			if(root->child->sibling!=NULL)
				printf("Array ID : %s with index : %s\n",root->child->tokenptr->lexeme.iden,root->child->sibling->tokenptr->lexeme.iden);
			else
				printf("Array ID : %s VARERROR \n",root->tokenptr->lexeme.iden);
		}
	}
	ASTNode *child=root->child;
	while(child!=NULL){
		testAST(child);
		child=child->sibling;
	}
}

void parseAST(ASTNode *root,HashTreeNode *htroot){
	/* LIST: 1. ID, VAR,MODULEDECLARATIONS
	/*			 2. CASESTMTS : type check var
	/*			 3. MODULE STMTS : CHECK INPUTLIST AND OUTPUTLIST
	/*			 4. EXPRESSIONS : CHECK TYPE
	/*       5. ARRAY : CHECK BOUNDS
	*/
	/*Add key to Symbol Table*/
	if(root->t==TERMINAL&&root->gnode.term==ID&&root->parent->gnode.non_term!=VAR){
		if(root->parent->stmttype==DECLARESTMT||
			root->parent->gnode.non_term==INPUTPLIST||
			root->parent->gnode.non_term==OUTPUTPLIST){
				printf("ID : %s with type : %d",root->tokenptr->lexeme.iden,root->dtype);
			if(htroot->parent!=NULL)
				printf("(child ht :%s) --> (%s)\n",htroot->table_name,htroot->parent->table_name);
			int st = addKey2(root,htroot);
			if(st==-1){
				printf("Previosly Declared ID redeclared.\n");
			}
		}
		else{
			printf("ID in source code: %s --> ",root->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) printf("ID not found in table :%s\n",htroot->table_name);
			else{
				printf("(dt:%d,ss:%d,es:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,entry->offset);
			}
		}
	}
	/*Add function declarations to symbol table*/
	if(root->t==NONTERMINAL&&root->gnode.non_term==MODULEDECLARATION){
		if(strcmp(htroot->table_name,"GLOBAL")!=0){
			printf("ERROR in module name insertion.\n");
		}
		int st = addKey2(root,htroot);
		if(st==-1) printf("Module redeclared.\n");
	}
	/*If not declaration check in symbol table for presence*/
	else if(root->t==NONTERMINAL&&root->gnode.non_term==VAR){
		if(root->vartype==1){
			printf("ID in source code: %s --> ",root->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) printf("ID not found in table :%s\n",htroot->table_name);
			else{
				printf("(dt:%d,ss:%d,es:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,entry->offset);
			}
		}
		else if(root->vartype==2){
			if(root->child->sibling!=NULL)
				printf("ID in source code: %s --> ",root->child->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->child->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) printf("ID not found in table :%s\n",htroot->table_name);
			else{
				printf("(dt:%d,ss:%d,es:%d,lr:%d,rr:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,
					entry->ast_node->lrange,entry->ast_node->rrange,entry->offset);
			}
		}
	}

	/*New Scope Generation*/
	if(root->t==NONTERMINAL&&(root->gnode.non_term==MODULENT||
		root->stmttype==CONDITIONALSTMT||
		root->stmttype==ITERATIVESTMT||
		root->gnode.non_term==DRIVERMODULE)){
		char function_name[LEXEME_SIZE];
		strcpy(function_name,htroot->function_name);
		HashTreeNode *global=htroot;
		htroot= addchild2(htroot);
		if(root->gnode.non_term==MODULENT||root->gnode.non_term==DRIVERMODULE){
			HashTableNode *entry=find2(root->tokenptr->lexeme.iden,global,1);
			if(entry!=NULL){
				if(entry->defined==0){
					entry->defined=1;
					printf("declared module : %s\n",entry->key);
				}
				else{
					printf("Module redefined.\n");
				}
			}
			else{
				int st= addKey2(root,global);
				entry= find2(root->tokenptr->lexeme.iden,global,1);
			}
			if(root->gnode.non_term==MODULENT){
				printf("Adding i/o lists.\n");
				add_plist(entry,root);
			}
			strcpy(htroot->table_name,root->tokenptr->lexeme.iden);
			strcpy(htroot->function_name,root->tokenptr->lexeme.iden);
		}
		else if(root->t==NONTERMINAL&&root->stmttype==ITERATIVESTMT){
			strcpy(htroot->table_name,root->looptype==FOR?"FOR":"WHILE");
			strcpy(htroot->function_name,function_name);
		}
		else{
			strcpy(htroot->table_name,"SWITCH");
			strcpy(htroot->function_name,function_name);
		}
	}

	/*Call to next nodes*/
	ASTNode *child=root->child;
	while(child!=NULL){
		parseAST(child,htroot);
		child=child->sibling;
	}
}

int main(int argc,char* argv[]){
	ASTNode *astroot;
	astroot= makeAST(argv[1]);
	// testAST(astroot);
	HashTreeNode *htroot= initTree();
	parseAST(astroot,htroot);
	return 0;
}