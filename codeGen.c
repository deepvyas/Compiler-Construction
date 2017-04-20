#include <stdio.h>
#include <stdlib.h>
#include "codeGenDef.h"

// extern ASTNode *astroot;
extern HashTreeNode *htroot;
	
void expression_cg(ASTNode *ast_root,FILE *fp){

}

void codegen(ASTNode *ast_root,HashTreeNode *htroot,FILE *fp){
	if(ast_root->gnode.non_term==DRIVERMODULE){
		fprintf(fp,"global driver\n");
		fprintf(fp,"section .text\n");
		fprintf(fp,"driver:\n");
		fprintf(fp, "mov rbp,rsp\n");
		printf("\n");
		ASTNode *it=ast_root->child;
		while(it!=NULL){
			codegen(it,fp);
			it=it->sibling;
		}
	}
	else if(ast_root->gnode.non_term==ASSIGNMENTSTMT){
		printf("Generating expression code first.\n");
		codegen(ast_root->child->sibling,fp);
		// Other logic here.
	}
	else if(ast_root->gnode.non_term==EXPRESSION){
		printf("Expression code starts.\n");
		expression_cg(ast_root,fp);
	}
	else{
		printf("Entering here : %d %d\n",ast_root->gnode.non_term,EXPRESSION);
		ASTNode *it=ast_root->child;
		while(it!=NULL){
			codegen(it,fp);
			it=it->sibling;
		}		
	}
}

int main(int argc,char* argv[]){
	ASTNode *astroot = makeAST(argv[1]);
	FILE *asmfile = fopen(argv[2],"w");
	codegen(astroot->child->sibling->sibling,htroot,asmfile);
	return 0;
}