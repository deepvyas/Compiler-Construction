// Batch Number - 38
// 2014A7PS102P - RISHABH JOSHI
// 2014A7PS248P - DEEP VYAS

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
#define encodingMul 1000000
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
        //printf("SOME PROBLEM!\n");
        return -1;
    }
    else {
        //printf("key = %s\n", entry->key);
        //printf("offset %d\n", entry->offset);
        return entry->offset;
    }
}

int pushOffset = 0;
int initialOffset = 0;
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
            astroot->decode=1;
        }
        else if (astroot->vartype == 0){
            int memLoc = pushOffset+initialOffset;
            pushOffset += INTEGEROFF;
            fprintf(fp, "\n; Var number to memory\n");
            fprintf(fp, "    mov qword [location+%d], %d\n", 
                    memLoc, astroot->value.num); 
            astroot->memoryLocation = memLoc;

        }
        if (astroot->sign == -1){
            fprintf(fp, ";negating var\n");
            fprintf(fp, "   mov r8, [location+%d]\n", astroot->memoryLocation+initialOffset);
            fprintf(fp, "   neg r8\n");
            int newMemLoc = pushOffset+initialOffset;
            pushOffset += INTEGEROFF;
            fprintf(fp, "   mov [location+%d], r8\n", newMemLoc);
            astroot->memoryLocation = newMemLoc;
        }
        dochild = 0;
    }
    // r8 and r9 fixed for the operators
    else if(astroot->t == TERMINAL && (astroot->gnode.term == PLUS || astroot->gnode.term == MINUS || astroot->gnode.term == MUL || astroot->gnode.term == DIV)){
        expression_cg(astroot->child, htroot);
        expression_cg(astroot->child->sibling, htroot);
        
        int memloc1 = astroot->child->memoryLocation+initialOffset, 
            memloc2 = astroot->child->sibling->memoryLocation+initialOffset;

        fprintf(fp, "\n;evaluating arith operator\n");
        int flag = 0;
        if (astroot->child->decode==1){
            int arr = memloc1/encodingMul;
            int indx = memloc1 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov r8, [location + r11*%d + %d]\n",INTEGEROFF,arr); 
            flag = 1;
        }
        if (astroot->child->sibling->decode==1){
            int arr = memloc2/encodingMul;
            int indx = memloc2 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov r9, [location + r11*%d + %d]\n",INTEGEROFF,arr);  
            flag = 1; 
        }
        if (!flag){
            fprintf(fp, "    mov r8, [location+%d]\n", memloc1); 
            fprintf(fp, "    mov r9, [location+%d]\n", memloc2); 
        }

        if (astroot->gnode.non_term == PLUS)
            fprintf(fp, "    add r8, r9\n"); 
        else if (astroot->gnode.non_term == MINUS)
            fprintf(fp, "    sub r8, r9\n"); 
        else if (astroot->gnode.non_term == MUL) 
            fprintf(fp, "    imul r8, r9\n"); 
        else if (astroot->gnode.non_term == DIV){
            fprintf(fp, "    xor rax, rax\n");// TO clear the contents
            fprintf(fp, "    xor rdx,rdx\n");
            fprintf(fp, "    mov rax, r8\n");
            fprintf(fp, "    idiv r9\n"); 
            fprintf(fp, "    mov r8, rax\n");// move back
        }
        int storeloc = pushOffset+initialOffset;
        pushOffset += INTEGEROFF;
        if(astroot->sign == -1){
            fprintf(fp, ";negating arith operator\n");
            fprintf(fp, "   neg r8\n");
        }
        fprintf(fp, "    mov [location+%d], r8\n", storeloc);
        astroot->memoryLocation = storeloc;
        dochild = 0;
    }
    else if(astroot->t == TERMINAL && (astroot->gnode.term == LE || astroot->gnode.term == LT || astroot->gnode.term == NE || astroot->gnode.term == GE || astroot->gnode.term == GT || astroot->gnode.term == EQ)){
        expression_cg(astroot->child, htroot);
        expression_cg(astroot->child->sibling, htroot);
        
        int memloc1 = astroot->child->memoryLocation+initialOffset, 
            memloc2 = astroot->child->sibling->memoryLocation + initialOffset;

        fprintf(fp, "\n;evaluating relational operator\n");
        int flag = 0;
        if (astroot->child->decode==1){
            int arr = memloc1/encodingMul;
            int indx = memloc1 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov r8, [location + r11*%d + %d]\n",INTEGEROFF,arr); 
            flag = 1;
        }
        if (astroot->child->sibling->decode==1){
            int arr = memloc2/encodingMul;
            int indx = memloc2 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov r9, [location + r11*%d + %d]\n",INTEGEROFF,arr);  
            flag = 1; 
        }
        if (!flag){
            fprintf(fp, "    mov r8, [location+%d]\n", memloc1); 
            fprintf(fp, "    mov r9, [location+%d]\n", memloc2); 
        }

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
        fprintf(fp, "    mov [location+%d], r8\n", storeloc);
        astroot->memoryLocation = storeloc;
        dochild = 0;
    }
    else if(astroot->t == TERMINAL && (astroot->gnode.term == AND || astroot->gnode.term == OR)){
        expression_cg(astroot->child, htroot);
        expression_cg(astroot->child->sibling, htroot);
        
        int memloc1 = astroot->child->memoryLocation + initialOffset, 
            memloc2 = astroot->child->sibling->memoryLocation + initialOffset;
        
        fprintf(fp, "\n;evaluating logical operator\n");
        int flag = 0;
        if (astroot->child->decode==1){
            int arr = memloc1/encodingMul;
            int indx = memloc1 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov r8, [location + r11*%d + %d]\n",INTEGEROFF,arr); 
            flag = 1;
        }
        if (astroot->child->sibling->decode==1){
            int arr = memloc2/encodingMul;
            int indx = memloc2 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov r9, [location + r11*%d + %d]\n",INTEGEROFF,arr);  
            flag = 1; 
        }
        if (!flag){
            fprintf(fp, "    mov r8, [location+%d]\n", memloc1); 
            fprintf(fp, "    mov r9, [location+%d]\n", memloc2); 
        }

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
        fprintf(fp, "    mov [location+%d], r8\n", storeloc);
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
		codegen(astroot->child->sibling);
        expression_cg(astroot->child, htroot);
        int memloc1 = astroot->child->memoryLocation + initialOffset;
        int memloc2 = astroot->child->sibling->memoryLocation+initialOffset;

        fprintf(fp, "\n; assignment\n");
        int flag = 0, flag2 = 0;
        if (astroot->child->sibling->decode==1){
            int arr = memloc2/encodingMul;
            int indx = memloc2 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov r8, [location + r11*%d + %d]\n",INTEGEROFF,arr);  
            flag = 1; 
        }
        if (astroot->child->decode==1){
            int arr = memloc1/encodingMul;
            int indx = memloc1 - arr*encodingMul;
            fprintf(fp, "    mov r11, [location+%d]\n", indx); 
            fprintf(fp, "    mov [location + r11*%d + %d], r8\n",INTEGEROFF,arr); 
            flag2 = 1;
        }
        if (flag == 0){
            fprintf(fp, "    mov r8, [location+%d]\n", memloc2);
        }
        if (flag2 == 0){
            fprintf(fp, "    mov [location+%d], r8\n", memloc1);
        }
		// Other logic here.
	}
	else if(astroot->gnode.non_term==EXPRESSION){
		//printf("Expression code starts.\n");
		expression_cg(astroot->child, htroot);
        astroot->memoryLocation = astroot->child->memoryLocation;
        astroot->decode = astroot->child->decode;
        //printf("value here : %d\n", astroot->memoryLocation);
	}
    else if(astroot->t == NONTERMINAL && astroot->gnode.non_term == STATEMENT && 
            astroot->stmttype == IOSTMT){
        if(astroot->child->gnode.term == GET_VALUE){
            HashTableNode *entry = find2(astroot->child->sibling->tokenptr->lexeme.iden, htroot,0);
            int datatype = entry->datatype;
            if (datatype == 3){
                ASTNode *idnode = astroot->child->sibling;
                int lrange = entry->ast_node->lrange, rrange = entry->ast_node->rrange;
                codegen(idnode);
                int memloc1 = idnode->memoryLocation + initialOffset;
                while(lrange != rrange+1){
                    fprintf(fp, "\n; getting array value\n");
                    fprintf(fp, "    push rbp\n");
                    fprintf(fp, "    mov rdi, formatin\n");
                    fprintf(fp, "    lea rsi, [location+%d]\n", memloc1);
                    fprintf(fp, "    xor rax, rax\n");
                    fprintf(fp, "    call scanf\n");
                    fprintf(fp, "    pop rbp\n");
                    memloc1 += INTEGEROFF;
                    lrange++;
                }
            }
            else {
                codegen(astroot->child->sibling);
                int memloc1 = astroot->child->sibling->memoryLocation + initialOffset;
                // Read value into memloc 1
                fprintf(fp, "\n; getting value\n");
                fprintf(fp, "    push rbp\n");
                fprintf(fp, "    mov rdi, formatin\n");
                fprintf(fp, "    lea rsi, [location+%d]\n", memloc1);
                fprintf(fp, "    xor rax, rax\n");
                fprintf(fp, "    call scanf\n");
                fprintf(fp, "    pop rbp\n");
            }
        }
        else{
            expression_cg(astroot->child->sibling, htroot);
            int memloc1 = astroot->child->sibling->memoryLocation + initialOffset;
            // print value in memloc 1

            fprintf(fp, "\n; printing value\n");
            fprintf(fp, "    push rbp\n");
            fprintf(fp, "    mov rdi, formatout\n");            

            int flag = 0;
            if (astroot->child->sibling->decode==1){
                int arr = memloc1/encodingMul;
                int indx = memloc1 - arr*encodingMul;
                fprintf(fp, "    mov r11, [location+%d]\n", indx); 
                fprintf(fp, "    mov rsi, [location + r11*%d + %d]\n",INTEGEROFF,arr); 
                flag = 1;
            }
            if (!flag){
                fprintf(fp, "    mov rsi, [location+%d]\n", memloc1);
            }           
            fprintf(fp, "    xor rax, rax\n");
            fprintf(fp, "    call printf\n");
            fprintf(fp, "    pop rbp\n");
        }
    }
    else if(astroot->t == NONTERMINAL && astroot->gnode.non_term == STATEMENT && 
            astroot->stmttype == ITERATIVESTMT&&astroot->looptype==FOR){
        codegen(astroot->child);
        fprintf(fp, "\n; For loop generation\n");
        fprintf(fp, "    mov r14, %d\n",astroot->child->lrange);
        fprintf(fp, "    mov r15, %d\n",astroot->child->rrange);
        fprintf(fp, "_for_%d:\n",labelNumber);
        int initlabel=labelNumber;
        codegen(astroot->child->sibling);
        fprintf(fp, "    add qword [location+%d], 1\n", astroot->child->memoryLocation+initialOffset);
        fprintf(fp, "    sub r15, 1\n");
        fprintf(fp, "    cmp r14, r15\n");
        fprintf(fp, "    jle _for_%d\n", initlabel);
        labelNumber++;
        fprintf(fp, "\n; For loop end\n");
    }
    else if(astroot->t == NONTERMINAL && astroot->gnode.non_term == STATEMENT && 
            astroot->stmttype == ITERATIVESTMT&&astroot->looptype==WHILE){
        fprintf(fp, "\n; While loop generation\n");
        fprintf(fp, "_while_%d:\n",labelNumber);
        int initlabel=labelNumber;
        //printf("While loop child :%d %d\n",astroot->child->t, astroot->child->gnode.non_term);
        expression_cg(astroot->child,htroot);
        fprintf(fp, "    mov r13,[location+%d]\n", astroot->child->memoryLocation+initialOffset);
        codegen(astroot->child->sibling);
        fprintf(fp, "    cmp r13, 0\n");
        fprintf(fp, "    jne _while_%d\n", initlabel);
        labelNumber++;
        fprintf(fp, "\n; While loop end\n");   
    }
    else if(astroot->t == NONTERMINAL && astroot->gnode.non_term == VAR){
        expression_cg(astroot,htroot);
    }
    else if(astroot->t == NONTERMINAL && astroot->gnode.non_term == STATEMENT && 
            astroot->stmttype == CONDITIONALSTMT){
        codegen(astroot->child);
        int memloc1=astroot->child->memoryLocation+initialOffset;
        fprintf(fp, "\n; Switch statement\n");
        int initlabel=labelNumber;
        fprintf(fp, "    mov r10, [location+%d]\n", memloc1);
        ASTNode *child=astroot->child->sibling->child;
        int value;
        while(child!=NULL){
            if(child->dtype==0){
                value = child->value.num;
            }
            else{
                value= child->value.tval;
            }
            fprintf(fp, "    cmp r10,%d\n",value);
            fprintf(fp, "    je _case_%d_%d_\n",initlabel,value);
            child = child->sibling;
        }

        if (astroot->child->sibling->sibling->child != NULL){
            fprintf(fp, "    jmp _case_%d_default_\n", initlabel);
        }
        else {
            fprintf(fp, "    jmp _case_%d_end_\n", initlabel);
        }

        child = astroot->child->sibling->child;
        while(child!=NULL){
            if(child->dtype==0){
                value = child->value.num;
            }
            else{
                value= child->value.tval;
            }
            fprintf(fp, "\n_case_%d_%d_:\n", initlabel, value);
            codegen(child);
            fprintf(fp, "    jmp _case_%d_end_\n", initlabel);
            child = child->sibling;
        }
        if (astroot->child->sibling->sibling->child != NULL){
            fprintf(fp, "\n_case_%d_default_:\n", initlabel);
            codegen(astroot->child->sibling->sibling->child);
        }
        fprintf(fp, "\n_case_%d_end_:\n", initlabel);
        fprintf(fp, "\n; Switch statement end\n");
        labelNumber++;
    }
	else{
		//printf("Entering here : %d %d\n",astroot->gnode.non_term,EXPRESSION);
		ASTNode *it=astroot->child;
		while(it!=NULL){
			codegen(it);
			it=it->sibling;
		}		
	}
}

//int main(int argc,char* argv[]){
	//fp = fopen(argv[2],"w");
    //int check = typeCheck(argv[1]);
    //ASTNode *astroot = ast_root;
    //if(check)
        //codegen(astroot->child->sibling->sibling);
    //// _printAST(ast_root);
    //fprintf(fp,"    mov rax, 0\n");
	//fprintf(fp,"    ret\n");
    //char format[] = "db \"%d\", 10, 0";
    //char formatin[] = "db \"%d\", 0";
	//fprintf(fp,"section .data\n");
	//fprintf(fp,"    formatout:    %s\n", format);
	//fprintf(fp,"    formatin:    %s\n", formatin);
    //fprintf(fp,"    location:    times 65536 db 0\n");
	//return 0;
//}
