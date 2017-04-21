#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"
#include "HashTreeDef.h"
#include "typeCheckingDef.h"

#define _integertype 0
#define _realtype 1
#define _booltype 2
#define _arrtype 3

int verbose = 1;

int globalItr = 0;
int globalItr2 = 0;
HashTreeNode *reuseInstancesHT[200];
int outputAssCheck[200];
int assCheckItr;

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
                printf("ERROR : Type match at line %d and are integer, but expression is logical.\n", node->tokenptr->line_no);
                return 0;
            }
        }
        else if (t1 == _realtype){
            if (ch == 'a')
                node->dtype = _realtype;
            else if(ch == 'r')
                node->dtype = _booltype;
            else {
                printf("ERROR : Type match at line %d and are real, but expression is logical.\n", node->tokenptr->line_no);
                return 0;
            }
        }
        else if (t1 == _booltype && ch == 'l'){
            node->dtype = _booltype;
        }
    }
    else {
        if (verbose){
            printf("ERROR : Type mismatch in expression %d and %d!\n", t1, t2);
            printf("Child : (%d,%d) and sibling (%d,%d)\n",node->child->t,node->child->gnode.non_term,
                node->child->sibling->t,node->child->sibling->gnode.non_term);
        }

        printf("ERROR : Type mismatch at line %d.\n", node->tokenptr->line_no);
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
    if (verbose)
        if(root->t == NONTERMINAL && root->gnode.non_term == STATEMENTS)
            printf("SHIT %d %d\n", root->dtype, root->parent->dtype);
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
                    if (verbose)
					    printf("declared module : %s\n",entry->key);
				}
				else{
					printf("ERROR : Module %s Redefined at line %d.\n", entry->key, root->tokenptr->line_no);////////////////////////
				}
			}
			else{
				int st= addKey2(root,global);
				entry= find2(root->tokenptr->lexeme.iden,global,1);
			}
			if(root->gnode.non_term==MODULENT){
				if(verbose)
                    printf("Adding i/o lists.\n");
				add_plist(entry,root);
			}
			strcpy(htroot->table_name,root->tokenptr->lexeme.iden);
			strcpy(htroot->function_name,root->tokenptr->lexeme.iden);
		}
		else if(root->t==NONTERMINAL&&root->stmttype==ITERATIVESTMT){
			strcpy(htroot->table_name,root->looptype==FOR?"FOR":"WHILE");
			strcpy(htroot->function_name,function_name);
            htroot->curr_offset = htroot->parent->curr_offset;
		}
		else{
			strcpy(htroot->table_name,"SWITCH");
			strcpy(htroot->function_name,function_name);
            htroot->curr_offset = htroot->parent->curr_offset;
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
				printf("ERROR : ID %s Redeclared at line %d.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no); ///////////////////////////
			}
            else{
                root->htPointer = htroot;
                // ID ke liye
            }
		}
		else{
//			printf("ID in source code: %s --> ",root->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) {
                printf("ERROR : ID %s at line %d not declared.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no); ///////////
                flag = 0;
            }
			else{
                if(verbose)
				    printf("(dt:%d,ss:%d,es:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,entry->offset);
			}
		}
/////////        flag = 1;
	}
	/*Add function declarations to symbol table*/
	if(root->t==NONTERMINAL&&root->gnode.non_term==MODULEDECLARATION){
		if(strcmp(htroot->table_name,"GLOBAL")!=0){
			printf("ERROR : Module %s's name not correctly inserted.\n", root->tokenptr->lexeme.iden); ////////////////////
            flag = 0;
		}
		int st = addKey2(root,htroot);
		if(st==-1) {
            printf("ERROR : Module %s redeclared at line %d.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no);/////////////////////////////
            flag = 0;
        }
    //////////    flag = 1;
	}
	/*If not declaration check in symbol table for presence*/
	else if(root->t==NONTERMINAL&&root->gnode.non_term==VAR){
        int type = 9;
		if(root->vartype==1){
//			printf("ID in source code: %s --> ",root->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->tokenptr->lexeme.iden,htroot,0);
			if(entry==NULL) {
                printf("ERROR : ID %s at line %d not declared.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no); ///////////
                flag = 0;
            }
			else{
                type = entry->datatype;
                if (root->parent->gnode.non_term == ASSIGNMENTSTMT)
                    entry->assignedFlag = 1;
                if(entry->readOnly == 1 && root->parent->gnode.non_term == ASSIGNMENTSTMT){
                    printf("ERROR : Loop index %s assigned in For Loop body at line %d!\n", entry->key, root->tokenptr->line_no);////
                    flag = 0;
                }
                if (verbose)
    				printf("(dt:%d,ss:%d,es:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,entry->offset);
			}
		}
		else if(root->vartype==2){
//          if(root->child->sibling!=NULL)
//				printf("ID in source code: %s --> ",root->child->tokenptr->lexeme.iden);
			HashTableNode *entry=find2(root->child->tokenptr->lexeme.iden,htroot,0);
            root->child->htPointer = htroot;
			if(entry==NULL) {
                printf("ERROR : ID %s at line %d not declared.\n", root->child->tokenptr->lexeme.iden, root->child->tokenptr->line_no); ///////////
                flag = 0;
            }
			else{
                //printf("array shit seen bitch %d %s!\n", entry->ast_node->arrtype, entry->key);
                type = entry->ast_node->arrtype;
                if (root->parent->gnode.non_term == ASSIGNMENTSTMT)
                    entry->assignedFlag = 1;
                if(entry->readOnly == 1 && root->parent->gnode.non_term == ASSIGNMENTSTMT){
                    printf("ERROR : Loop index %s assigned in For Loop body at line %d!\n", entry->key, root->child->tokenptr->line_no);////
                    flag = 0;
                }
                // check index
                HashTableNode *entry2 = find2(root->child->sibling->tokenptr->lexeme.iden,
                        htroot, 0);
                if (entry2 == NULL){
                    printf("ERROR : Loop Index %s of array %s at line %d not declared.\n", root->child->sibling->tokenptr->lexeme.iden, root->child->tokenptr->lexeme.iden, root->child->tokenptr->line_no); //////////////////////////////////////////////////////
                    flag = 0;
                }
                else if(entry2->datatype != _integertype){
                    printf("ERROR : Loop Index %s of array %s at line %d is not of type integer.\n", root->child->sibling->tokenptr->lexeme.iden, root->child->tokenptr->lexeme.iden, root->child->tokenptr->line_no); ////////////////////////////////////////////////////////////////////////
                    flag = 0;
                }
                if (verbose)
    				printf("(dt:%d,ss:%d,es:%d,lr:%d,rr:%d,off:%d)\n",entry->datatype,entry->startscope,entry->endscope,
					entry->ast_node->lrange,entry->ast_node->rrange,entry->offset);
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
        // Check recursion!
        if (strcmp(htroot->function_name, root->tokenptr->lexeme.iden) == 0){
            printf("ERROR : Recursive function call of %s within definition at line %d.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no); /////////
            flag = 0;
        }
        
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
        if (t1 != t2){
            printf("ERROR : Type mismatch at line %d.\n", root->child->tokenptr->line_no);/////////////////////////////
            flag = 0;
        }
        if (verbose)
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
                printf("ERROR : Loop Index %s of array %s at line %d not declared.\n", root->child->sibling->tokenptr->lexeme.iden, root->child->tokenptr->lexeme.iden, root->child->tokenptr->line_no); //////////////////////////////////////////////////////
                flag =0;
            }
            if (entry->datatype != _integertype) {
                printf("ERROR : Loop Index %s of array %s at line %d is not of type integer.\n", root->child->sibling->tokenptr->lexeme.iden, root->child->tokenptr->lexeme.iden, root->child->tokenptr->line_no); /////////////////////////////
                flag =0;
            }
            // Make the index readonly
            entry->readOnly = 1;
            root->child->htPointer = htroot;
            check = parseAST(root->child->sibling, htroot);
            if (check == 0) flag = 0;
            entry->readOnly = 0;
        }
        else { // while loop
            check = parseAST(root->child, htroot);
            chec2 = parseAST(root->child->sibling, htroot);
            if (root->child->dtype != _booltype){
                printf("ERROR : While loop condition at line %d is not of type boolean!\n", root->child->tokenptr->line_no);//////////
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
                printf("ERROR : ID %s at line %d not declared.\n", root->child->tokenptr->lexeme.iden, root->child->tokenptr->line_no); ///////////
                flag = 0;
            }
        }
        else {
            check = parseAST(root->child->sibling, htroot);
            if (check == 0) flag = 0;
        }
        root->child->htPointer = htroot;
        root->child->sibling->htPointer = htroot;
        dochild = 0;
    }
    else if (root->t == NONTERMINAL && root->gnode.non_term == STATEMENT &&
            root->stmttype == CONDITIONALSTMT){
		HashTableNode *entry=find2(root->child->tokenptr->lexeme.iden,htroot,0);
        root->child->htPointer = htroot;
        if (entry == NULL){
            printf("ERROR : ID %s at line %d not declared.\n", root->child->tokenptr->lexeme.iden, root->child->tokenptr->line_no); ///////////
            flag = 0;
        }
        int intorbool = 0; // 0 is int, 2 is bool
        if (verbose)
            printf("entry ka datatype! %d %s\n", entry->datatype, entry->key);
        if (entry->datatype == _integertype){
            intorbool = 0;
            if(root->child->sibling->sibling->child == NULL){
                printf("ERROR : No default statement but switch ID is integer at line %d!\n", root->child->tokenptr->line_no);/////////////////////////////////
                flag = 0;
            }
        }
        else if (entry->datatype == _booltype){
            intorbool = 2;
            if(root->child->sibling->sibling->child != NULL){
                printf("ERROR : Default statement exists at line %d but switch ID is boolean!\n", root->child->tokenptr->line_no);//////////////////////////////
                flag = 0;
            }
        }
        ASTNode *casestmts = root->child->sibling->child;

        int caseval[20];
        int j = 0, i;
        while(casestmts != NULL){
            if(verbose)
                printf("dtype %d and intorbool %d shiz is %d %d\n", casestmts->dtype, intorbool, casestmts->value.num,
                        casestmts->gnode.non_term);
            if (casestmts->dtype != intorbool){
                printf("ERROR : Case statement at line %d is not of type that is expected!\n", casestmts->tokenptr->line_no);////////////////////////////
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
                    printf("ERROR : Duplicate Case value at line %d.\n", casestmts->tokenptr->line_no);//////////////////////
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
            if(check == 0)
                flag = 0;
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
            root->gnode.term == GE || root->gnode.term == GT ||
            root->gnode.term == EQ || root->gnode.term == NE)){
        check = evaluateOperator(root, 'r');
        if (check == 0) flag = 0;
    }
    else if (root->t == TERMINAL && (root->gnode.term == AND || root->gnode.term == OR)){
        check = evaluateOperator(root, 'l');
        if (check == 0) flag = 0;
    }
    if (root->child != NULL && root->t == NONTERMINAL && root->gnode.non_term != VAR && 
        root->gnode.non_term != STATEMENTS)
        root->dtype = root->child->dtype;


    // Check the output plist assigned for moduleNT
    if (root->t == NONTERMINAL && root->gnode.non_term == MODULENT){
        int doneAss = 1;
        ASTNode *child = root->child->sibling->child;
		HashTableNode *entry;
        while (child != NULL){
            entry=find2(child->tokenptr->lexeme.iden,htroot,0);
            if (entry == NULL) flag = 0;
            else {
                if (entry->assignedFlag == 0){
                    printf("ERROR : The output parameter %s of module %s does not get assigned a value.\n", entry->key, root->tokenptr->lexeme.iden);////////////////////////////
                    flag = 0;
                }
            }
            child = child->sibling;
        }
    }

    if (verbose)
        if(root->t == NONTERMINAL && root->gnode.non_term == STATEMENTS)
            printf("SHIT2 %d %d\n", root->dtype, root->parent->dtype);
    // add htroot to the node's htPointer because SWAG
    root->htPointer = (HashTreeNode *)htroot;
    return flag;
}

int checkType(ASTNode *inpChild, ASTNode *inHT, HashTreeNode *htroot){
    HashTableNode *entry;
    entry = find2(inpChild->tokenptr->lexeme.iden, htroot, 0);
    if (entry == NULL){
        printf("ERROR : ID %s at line %d not declared.\n", inpChild->child->tokenptr->lexeme.iden, inpChild->child->tokenptr->line_no); ///////////
        return 0;
    }
    int type = inHT->dtype;
    int type2 = entry->datatype;
    if (type2 == _arrtype)
        type2 = entry->ast_node->arrtype;
    if (type == _arrtype)
        type = inHT->arrtype;
    if (type != type2){
        if (verbose)
            printf("The types are %d and %d  ", type, type2);
        return 0;
    }
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
        ASTNode **childArrIn = entry->input_plist;
        ASTNode **childArrOut = entry->output_plist;
        
        int tempx = 0;
        while(inpChild!=NULL){
            inpChild->htPointer = htroot;
            if (childArrIn[tempx] == NULL){
                printf("ERROR : Input parameters number mismatch of module %s at line %d.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no);/////////////////////////////////////////////////
                break;
            }
            check = checkType(inpChild, childArrIn[tempx], htroot);
            if (check == 0) {
                flag = 0;
                printf("ERROR : Input parameter %s type mismatch of module %s at line %d.\n", inpChild->tokenptr->lexeme.iden, root->tokenptr->lexeme.iden, root->tokenptr->line_no);/////////////////////////////////////////////////
            }
            tempx++;
            inpChild = inpChild->sibling;
        }
        if(inpChild == NULL && childArrIn[tempx] != NULL)
            printf("ERROR : Input parameters number mismatch of module %s at line %d.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no);/////////////////////////////////////////////////

        tempx = 0;
        inpChild = root->child->child;
        while(inpChild!=NULL){
            inpChild->htPointer = htroot;
            if (childArrOut[tempx] == NULL){
                printf("ERROR : Output parameters number mismatch of module %s at line %d.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no);/////////////////////////////////////////////////
                break;
            }
            check = checkType(inpChild, childArrOut[tempx], htroot);
            if (check == 0) {
                flag = 0;
                printf("ERROR : Output parameter %s type mismatch of module %s at line %d.\n", inpChild->tokenptr->lexeme.iden, root->tokenptr->lexeme.iden, root->tokenptr->line_no);/////////////////////////////////////////////////
            }
            tempx++;
            inpChild = inpChild->sibling;
        }
        if(inpChild == NULL && childArrOut[tempx] != NULL)
            printf("ERROR : Output parameters number mismatch of module %s at line %d.\n", root->tokenptr->lexeme.iden, root->tokenptr->line_no);/////////////////////////////////////////////////
        dochild = 0;
    }
    if (dochild){
        ASTNode *child=root->child;
        while(child!=NULL){
            check = parseASTAgain(child,globalHT);
            child=child->sibling;
        }
    }
    return flag;
}

