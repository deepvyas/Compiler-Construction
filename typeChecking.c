#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"
#include "HashTreeDef.h"

#define _integertype 0
#define _realtype 1
#define _booltype 2
#define _arrtype 3

int globalItr = 0;
int globalIt2 = 0;
HashTreeNode *reuseInstancesHT[200];

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

int evaluateOperator(ASTNode* node, char ch){
    // This function checks if all children have
    // ASSUMES that children have already been sorted and type checked
    // same type for arith to work
    // if That is the case, then put node->dtype or arr type accordingly 
    // return 1 if done
    // else 0
    int t1 = node->child->dtype;
    if (t1 == _arrtype) t1 = node->child->arrtype;
    int t2 = node->child->sibling->dtype;
    if (t2 == _arrtype) t2 = node->child->sibling->arrtype;
    if (t1 == t2){
        if (t1 == _integertype){
            if (ch == 'a')
                node->dtype = _integertype;
            else if (ch == 'r')
                node->dtype = _booltype;
            else {
                printf("Types in expr match, both int but expr is logical\n");
                return 0;
            }
        }
        else if (t1 == _realtype){
            if (ch == 'a')
                node->dtype = _realtype;
            else if(ch == 'r')
                node->dtype = _booltype;
            else {
                printf("Types in expr match, both real but expr is logical\n");
                return 0;
            }
        }
        else if (t1 == _booltype && ch == 'l'){
            node->dtype = _booltype;
        }
    }
    else {
        printf("Type mismatch in expression %d and %d!\n", t1, t2);
        return 0;
    }
    return 1;
}


int parseAST(ASTNode *root,HashTreeNode *htroot){
    // parseAST return 0 if parsing unsuccessful at the current node
    //    it returns 1 if curr node parsed successfully
	/* LIST: 1. ID, VAR,MODULEDECLARATIONS
	/*			 2. CASESTMTS : type check var
	/*			 3. MODULE STMTS : CHECK INPUTLIST AND OUTPUTLIST
	/*			 4. EXPRESSIONS : CHECK TYPE
	/*       5. ARRAY : CHECK BOUNDS
	*/
	/*Add key to Symbol Table*/

    int check; // for parseStatus
    int chec2;
    int flag = 1;
    int dochild = 1;
    //printf("Root nonterm  %d and statement type %d\n", root->gnode.non_term, root->stmttype);
	
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

	if(root->t==TERMINAL&&root->gnode.term==ID&&root->parent->gnode.non_term!=VAR){
		if(root->parent->stmttype==DECLARESTMT||
			root->parent->gnode.non_term==INPUTPLIST||
			root->parent->gnode.non_term==OUTPUTPLIST){
//				printf("ID : %s with type : %d",root->tokenptr->lexeme.iden,root->dtype);
//			if(htroot->parent!=NULL)
//				printf("(child ht :%s) --> (%s)\n",htroot->table_name,htroot->parent->table_name);
			int st = addKey2(root,htroot);
			if(st==-1){
				printf("Previosly Declared ID redeclared.\n");
			}
		}
		else{
//			printf("ID in source code: %s --> ",root->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) {
                printf("ID not found in table :%s\n",htroot->table_name);
                flag = 0;
            }
			else{
//				printf("(dt:%d,ss:%d,es:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,entry->offset);
			}
		}
/////////        flag = 1;
	}
	/*Add function declarations to symbol table*/
	if(root->t==NONTERMINAL&&root->gnode.non_term==MODULEDECLARATION){
		if(strcmp(htroot->table_name,"GLOBAL")!=0){
			printf("ERROR in module name insertion.\n");
            flag = 0;
		}
		int st = addKey2(root,htroot);
		if(st==-1) {
            printf("Module redeclared.\n");
            flag = 0;
        }
    //////////    flag = 1;
	}
	/*If not declaration check in symbol table for presence*/
	else if(root->t==NONTERMINAL&&root->gnode.non_term==VAR){
        int type;
		if(root->vartype==1){
//			printf("ID in source code: %s --> ",root->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) {
                printf("ID not found in table :%s\n",htroot->table_name);
                flag = 0;
            }
			else{
                type = entry->datatype;
//				printf("(dt:%d,ss:%d,es:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,entry->offset);
			}
		}
		else if(root->vartype==2){
//          if(root->child->sibling!=NULL)
//				printf("ID in source code: %s --> ",root->child->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->child->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) {
                printf("ID not found in table :%s\n",htroot->table_name);
                flag = 0;
            }
			else{
                //printf("array shit seen bitch %d %s!\n", entry->ast_node->arrtype, entry->key);
                type = entry->ast_node->arrtype;
                HashTableNode *entry2 = find2(root->child->sibling->tokenptr->lexeme.iden,
                        htroot, 0);
                if (entry2 == NULL){
                    printf("Index of array not found in the table: %s\n", htroot->table_name);
                    flag = 0;
                }
                else if(entry2->datatype != _integertype){
                    printf("Index of array is not of type integer!\n");
                    flag = 0;
                }
//				printf("(dt:%d,ss:%d,es:%d,lr:%d,rr:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,
//					entry->ast_node->lrange,entry->ast_node->rrange,entry->offset);
			}
		}
        else if (root->vartype == 0){
            type = root->dtype;
        }
        root->dtype = type;
        //printf("INSIDE VAR type : %d\n", type);
        dochild = 0;
////////        return 1;
	}

    // NEXT SPECIAL CASES
    // Module reuse statement to do in parse 2
    else if(root->t == NONTERMINAL && root->gnode.non_term == MODULEREUSESTMT){
        reuseInstancesHT[globalItr++] = htroot;
    }

    // Assignment statement
    // its like statement (simple) --child--> assignment stmt
    // and simple has child modulereuse
    // so only check non_term
    // and not stmnttype
    else if(root->t == NONTERMINAL && root->gnode.non_term == ASSIGNMENTSTMT){
        int t1, t2;
        check = parseAST(root->child, htroot);
        chec2 = parseAST(root->child->sibling, htroot);
        t1 = root->child->dtype;
        if (t1 == _arrtype)
            t1 = root->child->arrtype;
        t2 = root->child->sibling->dtype;
        if (t2 == _arrtype)
            t2 = root->child->arrtype;
        if (chec2 == 0 || check == 0) flag = 0;
        if (t1 != t2 && flag != 0){
            printf("Type mismatch in assignment statement!\n");
            flag = 0;
        }
        printf("In assignment statement types are %d and %d\n", t1, t2);
        dochild = 0;
    }
    // Iterative statement 
    else if(root->t == NONTERMINAL && root->gnode.non_term == STATEMENT && 
            root->stmttype == ITERATIVESTMT){
        if (root->looptype == FOR){
            // ALSO ADD READONLY FLAG
			HashTableNode *entry=find2(root->child->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) {
                printf("Index in for loop not found in table :%s\n",htroot->table_name);
                flag =0;
            }
            if (entry->datatype != _integertype) {
                printf("Index in for loop is not of type integer!\n");
                flag =0;
            }
            check = parseAST(root->child->sibling, htroot);
            if (check == 0) flag = 0;
        }
        else { // while loop
            check = parseAST(root->child, htroot);
            chec2 = parseAST(root->child->sibling, htroot);
            if (root->child->dtype != _booltype){
                printf("While loop condition not of type boolean!\n");
                flag = 0;
            }
            if (chec2 == 0 || check == 0)
                flag = 0;
        }
        dochild = 0;
    }
    // IO STMNT
    else if (root->t == NONTERMINAL && root->gnode.non_term == STATEMENT &&
            root->stmttype == IOSTMT){
        if (root->child->gnode.term == GET_VALUE){
			HashTableNode *entry=find2(root->child->sibling->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) {
                printf("ID in IOSTMNT not found in table :%s\n",htroot->table_name);
                flag = 0;
            }
        }
        else {
            check = parseAST(root->child->sibling, htroot);
            if (check == 0) flag = 0;
        }
        dochild = 0;
    }
    else if (root->t == NONTERMINAL && root->gnode.non_term == STATEMENT &&
            root->stmttype == CONDITIONALSTMT){
		HashTableNode *entry=find2(root->child->tokenptr->lexeme.iden,htroot,0);
        if (entry == NULL){
            printf("ID in CONDITIONAL not found in table :%s\n",htroot->table_name);
            flag = 0;
        }
        int intorbool = 0; // 0 is int, 2 is bool
        printf("entry ka datatype! %d %s\n", entry->datatype, entry->key);
        if (entry->datatype == _integertype){
            intorbool = 0;
            if(root->child->sibling->sibling->child == NULL){
                printf("No default statement but id is integer!\n");
                flag = 0;
            }
        }
        else if (entry->datatype == _booltype){
            intorbool = 2;
            if(root->child->sibling->sibling->child != NULL){
                printf("Default statement exists but id is boolean!\n");
                flag = 0;
            }
        }
        ASTNode *casestmts = root->child->sibling->child;

        int caseval[20];
        int j = 0, i;
        while(casestmts != NULL){
            if (casestmts->dtype != intorbool){
                printf("Case statement is not of type that matches with ID!\n");
                flag = 0;
            }
            if (casestmts->dtype == _integertype)
                caseval[j++] = casestmts->value.num;
            else
                caseval[j++] = casestmts->value.tval;
            // check if arr already has this shit
            i = 0;
            for(i= 0 ; i < j-1; i++){
                if (caseval[i] == caseval[j-1]){
                    printf("Case value already exits\n");
                }
            }
            check = parseAST(casestmts->child, htroot);
            if (check == 0) flag = 0;
            casestmts = casestmts->sibling;
        }
        // for default
        casestmts = root->child->sibling->sibling->child;
        while(casestmts != NULL){
            check = parseAST(casestmts, htroot);
            if (check == 0) flag = 0;
            casestmts = casestmts->sibling;
        }

        
        dochild = 0;
    }
    
	/*Call to next nodes*/
    if (dochild == 1){
        ASTNode *child=root->child;
        while(child!=NULL){
            check = parseAST(child,htroot);
            child=child->sibling;
        }
    }

    // Now the children have been processed. Next for expressions!!
    if (root->t == TERMINAL && (root->gnode.term == PLUS || root->gnode.term == MINUS ||
            root->gnode.term == MUL || root->gnode.term == DIV)){
        check = evaluateOperator(root, 'a');
        if (check == 0) flag = 0;
    }
    else if (root->t == TERMINAL && (root->gnode.term == LT || root->gnode.term == LE ||
            root->gnode.term == GE || root->gnode.term == GT &&
            root->gnode.term == EQ || root->gnode.term == NE)){
        check = evaluateOperator(root, 'r');
        if (check == 0) flag = 0;
    }
    else if (root->t == TERMINAL && (root->gnode.term == AND || root->gnode.term == OR)){
        check = evaluateOperator(root, 'l');
        if (check == 0) flag = 0;
    }
    if (root->child != NULL && root->t == NONTERMINAL && root->gnode.non_term != VAR)
        root->dtype = root->child->dtype;

    return flag;
}

int checkType(ASTNode *inpChild, ASTNode *inHT, HashTreeNode *htroot){
    HashTableNode *entry;
    entry = find2(inpChild->tokenptr->lexeme.iden, htroot, 0);
    if (entry == NULL){
        printf("Id not present in the Symbol Table.\n");
        return 0;
    }
    int type = inHT->dtype;
    if (type == _arrtype)
        type = inHT->arrtype;
    if (type != entry->datatype)
        return 0;
    return 1;
}

int parseASTAgain(ASTNode *root, HashTreeNode *globalHT){
    // Use this fucntion for only module reuse
    int dochild = 1;
    int flag = 1;
    int check;
    if (root->t == NONTERMINAL && root->gnode.non_term == MODULEREUSESTMT){
        HashTreeNode *htroot = reuseInstancesHT[globalItr2++];
        HashTableNode *entry;
        ASTNode *inpChild = root->child->sibling->child;
        entry = find2(root->tokenptr->lexeme.iden, globalHT, 1);
        ASTNode *childArrIn = entry->input_plist;
        ASTNode *childArrOut = entry->output_plist;
        
        int tempx = 0;
        while(inpChild!=NULL){
            if (childArrIn[tempx] == NULL){
                printf("The numbers of parameters dont match for input parameters of module reuse!"\n);
                break;
            }
            check = checkType(inpChild, childArrIn[tempx], htroot);
            if (check == 0) {
                flag = 0;
                printf("Type mismatch in module reuse input parameters!\n");
            }
            tempx++;
            inpChild = inpChild->sibling;
        }

        tempx = 0;
        inpChild = root->child->child;
        while(inpChild!=NULL){
            if (childArrOut[tempx] == NULL){
                printf("The numbers of parameters dont match for return parameters of module reuse!"\n);
                break;
            }
            check = checkType(inpChild, childArrOut[tempx], htroot);
            if (check == 0) {
                flag = 0;
                printf("Type mismatch in module reuse return parameters!\n");
            }
            tempx++;
            inpChild = inpChild->sibling;
        }
        dochild = 0;
    }
    if (dochild){
        ASTNode *child=root->child;
        while(child!=NULL){
            check = parseASTAgain(child,htroot);
            child=child->sibling;
        }
    }
    return flag;
}

int main(int argc,char* argv[]){
	ASTNode *astroot;
	astroot= makeAST(argv[1]);
	// testAST(astroot);
	HashTreeNode *htroot= initTree();
	parseAST(astroot,htroot);
	return 0;
}
