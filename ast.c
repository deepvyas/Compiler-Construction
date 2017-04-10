#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"


ASTNode* create_ast_node(){
	ASTNode* node;
	node = (ASTNode*)malloc(1*sizeof(ASTNode));
	return node;
}

int extract_type(ParseTreeNode *node){
	int type= node->left->nodeSymbol.ele.term;
	if(type==INTEGER) return 0;
	else if(type==REAL) return 1;
	else if(type==BOOLEAN) return 2;
	else if(type==ARRAY) return 3;
}

ASTNode* make_id_node(ParseTreeNode *idnode,ParseTreeNode *dnode){
	ASTNode *head;
	head= create_ast_node();
	head->t=TERMINAL;
	head->gnode.term=idnode->nodeSymbol.ele.term;
	head->tokenptr=idnode->tokenptr;
	head->dtype = extract_type(dnode);
	if(head->dtype==3){
		ParseTreeNode *rangenode=dnode->left->sibling->sibling;
		ParseTreeNode *it = dnode->left;
		while(it->sibling!=NULL){
			it=it->sibling;
		}
		head->arrtype= extract_type(it);
		head->lrange= rangenode->left->tokenptr->lexeme.num;
		head->rrange= rangenode->left->sibling->sibling->tokenptr->lexeme.num;
	}
	return head;
}

ASTNode* compressList(ParseTreeNode *proot,ASTNode *parent){
	ASTNode *head;
	ParseTreeNode *itr=proot;
	int nt= proot->nodeSymbol.ele.non_term;
	if(nt==INPUTPLIST||nt==OUTPUTPLIST){
		ParseTreeNode *typenode = itr->left->sibling->sibling;
		head = make_id_node(itr->left,typenode);
		head->parent=parent;
		ParseTreeNode *n1=typenode->sibling;
		ASTNode *ast_it=head;
		while(n1->left->nodeSymbol.ele.term!=EPSILON){
			ParseTreeNode *temp=n1->left->sibling;
			ast_it->sibling=make_id_node(temp,temp->sibling->sibling);
			ast_it->sibling->parent=parent;
			ast_it=ast_it->sibling;
			n1=temp->sibling->sibling->sibling;
		}
		return head;
	}
	else{
		if(proot->left->nodeSymbol.t==TERMINAL&&proot->left->nodeSymbol.ele.term==EPSILON){
		head = create_ast_node();
		head->t=TERMINAL;
		head->gnode.term=EPSILON;
		head->parent=parent;
		head->child=NULL;
		head->sibling=NULL;
		return head;
		}
		if(itr->nodeSymbol.t==NONTERMINAL){
		head = genAST(proot->left,parent);
		head->parent=parent;
		}
		itr=itr->left->sibling;
		ASTNode *ast_it=head;
		while(itr->left->nodeSymbol.ele.term!=EPSILON){
			ast_it->sibling = genAST(itr->left,parent);
			ast_it=ast_it->sibling;
			itr=itr->left->sibling;
		}
	}
	return head;
}


ASTNode* genAST(ParseTreeNode *proot,ASTNode *parent){
	if(proot==NULL) return NULL;
	/*Program Non-Terminal which stores all child*/
	ASTNode *root = create_ast_node();
	root->t=proot->nodeSymbol.t;
	root->gnode.non_term=proot->nodeSymbol.ele.non_term;
	root->parent=parent;
	root->tokenptr=NULL;
	if(proot->nodeSymbol.t==NONTERMINAL && proot->nodeSymbol.ele.non_term==PROGRAMNT){
		root->child=genAST(proot->left,root);
		ASTNode *ast_it=root->child;
		ParseTreeNode *it=proot->left;
		while(it!=NULL){
			ast_it->sibling = genAST(it->sibling,root);
			ast_it=ast_it->sibling;
			it=it->sibling;
		}
		return root;
	}
	else if(proot->nodeSymbol.t==NONTERMINAL&&
				(proot->nodeSymbol.ele.non_term==MODULEDECLARATIONS||
				proot->nodeSymbol.ele.non_term==OTHERMODULES)){
		root->child = compressList(proot,root);
		return root;
	}
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==MODULEDECLARATION){
		root->child=NULL;
		root->tokenptr = proot->left->sibling->sibling->tokenptr;
		// Enter into Symbol Table;
		return root;
	}
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==DRIVERMODULE){
		root->tokenptr = proot->left->sibling->tokenptr;
		root->scope=1; //TODO : SCOPE NUMBER FIXING
		ParseTreeNode *it=proot->left;
		while(it->sibling!=NULL){
			it=it->sibling;
		}
		root->child = genAST(it,root);
		return root;
	}
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==MODULENT){
		ParseTreeNode *it = proot->left;
		root->tokenptr= it->sibling->sibling->tokenptr;
		root->scope = 1; //TODO : SCOPE NUMBER FIXING
		while(it->nodeSymbol.ele.non_term!=INPUTPLIST) it=it->sibling;
		root->child=	genAST(it,root);
		//RET
		it=it->sibling->sibling->sibling;
		if(it->left->nodeSymbol.ele.term==EPSILON){
			root->child->sibling = create_ast_node();
			root->child->sibling->gnode.non_term=it->nodeSymbol.ele.non_term;
			root->child->sibling->t=it->nodeSymbol.t;
			root->child->sibling->child= create_ast_node();
			root->child->sibling->child->t= TERMINAL;
			root->child->sibling->child->gnode.term= EPSILON;
		}
		else{
			root->child->sibling = genAST(it->left->sibling->sibling,root);
		}
		root->child->sibling->sibling = genAST(it->sibling,root);
		return root;
	}
	else if(proot->nodeSymbol.t==NONTERMINAL&&
		(proot->nodeSymbol.ele.non_term==INPUTPLIST||proot->nodeSymbol.ele.non_term==OUTPUTPLIST)){
		root->child = compressList(proot,root);
		return root;
	}
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==MODULEDEF){
		root->child = genAST(proot->left->sibling,root);
		root->scope= 1; // ADD SCOPE;
		return root;
	}
	/*TODO : Statements and Beyond */



	else{
		ASTNode *root = create_ast_node();
		root->t=proot->nodeSymbol.t;
		root->gnode.non_term=proot->nodeSymbol.ele.non_term;
		root->parent=parent;
		return root;
	}
}

void _printAST(ASTNode *ast_root){
	if(ast_root==NULL) return;
	if(ast_root->child!=NULL)
		printf("Child of %d : %d",ast_root->gnode.non_term,ast_root->child->gnode.non_term);
	else{
		if(ast_root->tokenptr!=NULL){
			if(ast_root->gnode.term==ID){
				printf("Token at %d : %s and datatype is %d\n",ast_root->gnode.non_term,ast_root->tokenptr->lexeme.iden,
					ast_root->dtype);
			}
			else{
				printf("Token at %d : %s\n",ast_root->gnode.non_term,ast_root->tokenptr->lexeme.iden);
			}
		}
		else{
			printf("Child of %d :\n",ast_root->gnode.non_term);
		}
		return;
	}
	ASTNode *it=ast_root->child->sibling;
	while(it!=NULL){
		printf(", %d",it->gnode.non_term);
		it=it->sibling;
	}
	printf("\n");
	it=ast_root->child;
	while(it!=NULL){
		_printAST(it);
		it=it->sibling;
	}
}
int main(int argc,char* argv[]){
	initTable();
	populateGrammar();
	compute_first();
	compute_follow();
	createParsingTable();
	ParseTreeNode *proot=parseInputSourceCode(argv[1],0);
	ASTNode *ast_root=genAST(proot,NULL);
	_printAST(ast_root);
	return 0;
}