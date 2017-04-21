#include <stdio.h>
#include <stdlib.h>
#include "codeGenDef.h"
#include "typeCheckingDef.h"

// extern ASTNode *astroot;
//extern HashTreeNode *htroot;

ASTNode *ast_root;
HashTreeNode *htroot;
FILE *fp;

#define _arrtype 3
#define encodingMul 100000
#define INTEGEROFF 8

int typeCheck(char *filename){
    ast_root = makeAST(filename);
    htroot = initTree();
    int check = 1, flag = 1, check2 = 1;
    check = parseAST(ast_root, htroot);
    if (check == 0)
        flag = 0;
    check2 = parseASTAgain(ast_root, htroot);
    if (check2 == 0)
        flag = 0;
    return flag;
}

int getOffset(ASTNode *astroot, HashTreeNode *htroot){
    HashTableNode *entry; 
    entry = find2(astroot->tokenptr->lexeme.iden, htroot, 0);
    if (entry == NULL) {
        printf("SOME PROBLEM!\n");
        return -1;
    }
    else {
        printf("offset %d\n", entry->offset);
        return entry->offset;
    }
}

int pushOffset = 0;
int initialOffset = 1000;
int labelNumber = 0;

void expression_cg(ASTNode *astroot, HashTreeNode *htroot){
    int offset;
    int dochild = 1;
    //printf("ASTROOT : %d\n", astroot->gnode.non_term);
    if (astroot->t == NONTERMINAL && astroot->gnode.non_term == VAR){
        if (astroot->vartype == 1){ //ID
            astroot->memoryLocation = getOffset(astroot, htroot);
        }
        else if(astroot->vartype == 2){ //array
            offset = getOffset(astroot->child, htroot); // memloc of arr
            offset = offset*encodingMul + getOffset(astroot->child->sibling, htroot); // memloc of index 2
            astroot->memoryLocation = offset;
        }
        if (astroot->sign == -1){
            fprintf(fp, ";negating var\n");
            fprintf(fp, "   mov r8, [rbp-%d]\n", astroot->memoryLocation+initialOffset);
            fprintf(fp, "   neg r8\n");
            int newMemLoc = pushOffset+initialOffset;
            pushOffset += INTEGEROFF;
            fprintf(fp, "   mov [rbp-%d], r8\n", newMemLoc);
            astroot->memoryLocation = newMemLoc;
        }
        dochild = 0;
    }
    // r8 and r9 fixed for the operators
    else if(astroot->t == TERMINAL && (astroot->gnode.term == PLUS || 
                astroot->gnode.term == MINUS || astroot->gnode.term == MUL || 
                astroot->gnode.term == DIV)){
        expression_cg(astroot->child, htroot);
        expression_cg(astroot->child->sibling, htroot);
        
        int memloc1 = astroot->child->memoryLocation+initialOffset, 
            memloc2 = astroot->child->sibling->memoryLocation+initialOffset;
        fprintf(fp, "\n;evaluating arith operator\n");
        fprintf(fp, "    mov r8, [rbp-%d]\n", memloc1); 
        fprintf(fp, "    mov r9, [rbp-%d]\n", memloc2); 

        if (astroot->gnode.non_term == PLUS)
            fprintf(fp, "    add r8, r9\n"); 
        else if (astroot->gnode.non_term == MINUS)
            fprintf(fp, "    sub r8, r9\n"); 
        else if (astroot->gnode.non_term == MUL) 
            fprintf(fp, "    imul r8, r9\n"); 
        else if (astroot->gnode.non_term == DIV){
            fprintf(fp, "    xor rax, rax\n");// TO clear the contents
            fprintf(fp, "    mov rax, r8w\n");
            fprintf(fp, "    idiv r9\n"); 
            fprintf(fp, "    mov r8, rax\n");// move back
        }
        int storeloc = pushOffset+initialOffset;
        pushOffset += INTEGEROFF;
        if(astroot->sign == -1){
            fprintf(fp, ";negating arith operator\n");
            fprintf(fp, "   neg r8\n");
        }
        fprintf(fp, "    mov [rbp-%d], r8\n", storeloc);
        astroot->memoryLocation = storeloc;
        dochild = 0;
    }
    else if(astroot->t == TERMINAL && (astroot->gnode.term == LE || 
                astroot->gnode.term == LT || astroot->gnode.term == NE || 
                astroot->gnode.term == GE || astroot->gnode.term == GT ||
                astroot->gnode.term == EQ)){
        expression_cg(astroot->child, htroot);
        expression_cg(astroot->child->sibling, htroot);
        
        int memloc1 = astroot->child->memoryLocation+initialOffset, 
            memloc2 = astroot->child->sibling->memoryLocation + initialOffset;
        fprintf(fp, "\n;evaluating relational operator\n");
        fprintf(fp, "    mov r8, [rbp-%d]\n", memloc1); 
        fprintf(fp, "    mov r9, [rbp-%d]\n", memloc2); 

        fprintf(fp, "    cmp r8, r9\n");
        if (astroot->gnode.non_term == LE)
            fprintf(fp, "    jle _true_%d\n", labelNumber); 
        else if (astroot->gnode.non_term == LT)
            fprintf(fp, "    jl _true_%d\n", labelNumber); 
        else if (astroot->gnode.non_term == NE) 
            fprintf(fp, "    jne _true_%d\n", labelNumber); 
        else if (astroot->gnode.non_term == EQ)
            fprintf(fp, "    je _true_%d\n", labelNumber); 
        else if (astroot->gnode.non_term == GE)
            fprintf(fp, "    jge _true_%d\n", labelNumber); 
        else if (astroot->gnode.non_term == GT)
            fprintf(fp, "    jg _true_%d\n", labelNumber); 
        fprintf(fp, "_false_%d:\n    mov r8, 0\n    jmp _continue_%d\n", labelNumber, labelNumber);
        fprintf(fp, "_true_%d:\n    mov r8, 1\n", labelNumber);
        fprintf(fp, "_continue_%d:\n    \n", labelNumber++);
        int storeloc = pushOffset+initialOffset;
        pushOffset += INTEGEROFF;
        if(astroot->sign == -1){
            fprintf(fp, ";negating relational operator\n");
            fprintf(fp, "   neg r8\n");
        }
        fprintf(fp, "    mov [rbp-%d], r8\n", storeloc);
        astroot->memoryLocation = storeloc;
        dochild = 0;
    }
    else if(astroot->t == TERMINAL && (astroot->gnode.term == AND || 
                astroot->gnode.term == OR)){
        expression_cg(astroot->child, htroot);
        expression_cg(astroot->child->sibling, htroot);
        
        int memloc1 = astroot->child->memoryLocation + initialOffset, 
            memloc2 = astroot->child->sibling->memoryLocation + initialOffset;
        fprintf(fp, "\n;evaluating logical operator\n");
        fprintf(fp, "    mov r8, [rbp-%d]\n", memloc1); 
        fprintf(fp, "    mov r9, [rbp-%d]\n", memloc2); 

        if (astroot->gnode.non_term == AND)
            fprintf(fp, "    and r8, r9\n"); 
        else if (astroot->gnode.non_term == OR)
            fprintf(fp, "    or r8, r9\n"); 
        int storeloc = pushOffset + initialOffset;
        pushOffset += INTEGEROFF;
        if(astroot->sign == -1){
            fprintf(fp, ";negating logical operator\n");
            fprintf(fp, "   neg r8\n");
        }
        fprintf(fp, "    mov [rbp-%d], r8\n", storeloc);
        astroot->memoryLocation = storeloc;
        dochild = 0;
    }
    if (dochild){
        ASTNode *tempemp = astroot->child;
        while(tempemp != NULL){
            expression_cg(tempemp, htroot);
            tempemp = tempemp->sibling;
        }
    }
//    ASTNode *children = astroot->child;
//    expression_cg(children);
    
}

void codegen(ASTNode *astroot){
    HashTreeNode *htroot = (HashTreeNode*)astroot->htPointer;

	if(astroot->gnode.non_term==DRIVERMODULE){
        pushOffset = ((HashTreeNode *)astroot->htPointer)->curr_offset;
		fprintf(fp,"global main\n");
		fprintf(fp,"extern printf\n");
		fprintf(fp,"extern scanf\n");
		fprintf(fp,"section .text\n");
		fprintf(fp,"main:\n");
		fprintf(fp, "    mov rbp,rsp\n");
		ASTNode *it=astroot->child;
		while(it!=NULL){
			codegen(it);
			it=it->sibling;
		}
	}
    else if(astroot->t == TERMINAL && astroot->gnode.term == ID){
        astroot->memoryLocation = getOffset(astroot, htroot);
    }
	else if(astroot->gnode.non_term==ASSIGNMENTSTMT){
		printf("Generating expression code first.\n");
		codegen(astroot->child->sibling);
		// Other logic here.
	}
	else if(astroot->gnode.non_term==EXPRESSION){
		printf("Expression code starts.\n");
		expression_cg(astroot->child, htroot);
	}
    else if(astroot->t == NONTERMINAL && astroot->gnode.non_term == STATEMENT && 
            astroot->stmttype == IOSTMT){
        if(astroot->child->gnode.term == GET_VALUE){
            codegen(astroot->child->sibling);
            int memloc1 = astroot->child->sibling->memoryLocation + initialOffset;
            // Read value into memloc 1
            fprintf(fp, "\n; getting value\n");
            fprintf(fp, "    push rbp\n");
            fprintf(fp, "    mov rdi, formatin\n");
            fprintf(fp, "    mov rsi, location\n");
            fprintf(fp, "    xor rax, rax\n");
            fprintf(fp, "    call scanf\n");
            fprintf(fp, "    pop rbp\n");
            fprintf(fp, "    mov rax, [location]\n");
            fprintf(fp, "    mov [rbp-%d], rax\n", memloc1);
        }
        else{
            expression_cg(astroot->child->sibling, htroot);
            int memloc1 = astroot->child->sibling->memoryLocation + initialOffset;
            // print value in memloc 1
            fprintf(fp, "\n; printing value\n");
            fprintf(fp, "    push rbp\n");
            fprintf(fp, "    mov rdi, formatout\n");
            fprintf(fp, "    mov rsi, [rbp-%d]\n", memloc1); 
            fprintf(fp, "    xor rax, rax\n");
            fprintf(fp, "    call printf\n");
            fprintf(fp, "    pop rbp\n");
        }
    }
	else{
		printf("Entering here : %d %d\n",astroot->gnode.non_term,EXPRESSION);
		ASTNode *it=astroot->child;
		while(it!=NULL){
			codegen(it);
			it=it->sibling;
		}		
	}
}

int main(int argc,char* argv[]){
	fp = fopen(argv[2],"w");
    int check = typeCheck(argv[1]);
    ASTNode *astroot = ast_root;
    if(check)
        codegen(astroot->child->sibling->sibling);
    //_printAST(ast_root);
    fprintf(fp,"    mov rax, 0\n");
	fprintf(fp,"    ret\n");
    char format[] = "db \"%d\", 10, 0";
    char formatin[] = "db \"%d\", 0";
	fprintf(fp,"section .data\n");
	fprintf(fp,"    formatout:    %s\n", format);
	fprintf(fp,"    formatin:    %s\n", formatin);
    fprintf(fp,"    location:    times 8 db 0\n");
	return 0;
}
