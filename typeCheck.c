#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"

#define _integertype 0
#define _realtype 1
#define _booltype 2
#define _arrtype 3

typedef int TYPE

// TODE change evaluate functions
int evaluateOperator(ASTNode* node, char ch){
    // This function checks if all children have
    // ASSUMES that children have already been sorted
    // same type for arith to work
    // if That is the case, then put node->dtype or arr type accordingly 
    // return 1 if done
    // else 0
    TYPE t1 = node->lop->dytpe;
    if (t1 == _arrtype) t1 = node->lop->arrtype;
    TYPE t2 = node->rop->dytpe;
    if (t2 == _arrtype) t2 = node->rop->arrtype;
    if (t1 == t2){
        if (t1 == _integertype){
            if (ch == 'a')
                node->dtype = _integertype;
            else if (ch == 'r')
                node->dtype = _booltype;
            else return 0
        }
        else if (t1 == _realtype){
            if (ch == 'a')
                node->dtype = _realtype;
            else if(ch == 'r')
                node->dtype = _booltype;
            else return 0;
        }
        else if (t1 == _booltype && ch == 'l'){
            node->dtype = _booltype;
        }
        else return 0;
    }
    else return 0;
    return 1;
}

int insertInSymbolTable(ASTNode *node, environment){

}

int checkInSymbolTable(ASTNode *node, environment){

}

int updateSymbolTable(ASTNode *node, environment){

}

int typechecking(ASTNode* node, environment){
    // Funcition evaluates the correct type at the node and
    // verifies it with dtype field already present in the ASTNode
    // If it matches, it returns 1, otherwise 0
    
    // To find number of children
    ASTNode *tempPtr = node->child;
    int numChilds = 0, check = 0;
    whlie(tempPtr!=NULL){
        numChilds++;
        tempPtr = tempPtr->sibling;
    }
    // Now numChilds has the number of children of the node

    // SPECIAL CASES
    if (numChilds == 0 || node->tokenPtr != NULL){ // CASES HERE
        if (node->t == NONTERMINAL && 
                node->gnode.non_term == MODULEDECLARATIONS){
            check = insertInSymbolTable(node, environment);
            if (check == 0) // already inserted in same scope
                return 0;
            return 1;
        }
        else if (node->t == NONTERMINAL &&
                node->gnode.non_term == DRIVERMODULE){
            check = insertInSymbolTable(node, environment); 
            // add driver in ST
            if (check == 0) return 0;
            return 1;
        }
        else if (node->t == NONTERMINAL &&
                node->gnode.non_term == MODULENT){
            // HAVE TO ASSIGN FLAGS TO OUTPLIST
            // Handle differently as inputplist and outplist have to be added
            // along with check for declaration
            check = updateSymbolTable(node, environment);
            if (check == 0) return 0;
            // CHECK TO ASSIGN FLAGS TO OUTPLIST
            return 1;
        }
        else if (node->t == NONTERMINAL &&
                node->gnode.non_term == VAR){
            check = checkInSymbolTable(node, environment);
            if (check == 0) return 0;
            // node dytpe already populated
            return 1;
        }
        else if (node->t == TERMINAL &&
                node->gnode.term == ID){
            // check =
            // DONT REMEMBER WHAT TO DO
        }
        else if (node->t == NONTERMINAL &&
                node->gnode.non_term == MODULEREUSESTMT){
            // Epic shit here to check corresponding lists
            
        }
        // I think this was for standard id
        // Check here for redeclaration
        // Check for present in symbol table
        // Check for scope
        // Check if tokenpointer
        check = checkInSymbolTable(node, environment);
        if (check == 0) return 0; // Some error
        // 4 types of declares handled:
        // NUM then type already integer
        // RNUM then type already real
        // true/flase then type already boolean
        // array type already known too
        return 1;
    }
    else if (node->t == NONTERMINAL &&
            node->gnode.non_term == VAR){
        // This one is for array var. The other is handled above
        check = checkInSymbolTable(node->child, environment);
        if (check == 0) return 0;
        check = checkInSymbolTable(node->child->sibling, environment);
        if (check == 0) return 0;
        node->dtype = node->child->arrtype; // type of array is in arrtype
        // DO CHECK HERE
        return 1;
    }
    else if (node->t == NONTERMINAL && 
            node->gnode.non_term == ASSIGNMENTSTMT){
        TYPE t1, t2;
        check = typechecking(node->child, environment);
        if (check == 0) return 0;
        t1 = node->child->dtype;
        if (t1 == _arrtype)
            t1 = node->child->arrtype;
        check = typechecking(node->child->sibling, environment);
        if (check == 0) return 0;
        t2 = node->child->sibling->dtype;
        if (t1 != t2)
            return 0;
        return 1;
    }
    else if (node->t == NONTERMINAL && 
            node->gnode.non_term == CONDITIONALSTMT){
        // Do epic shit here

    }
    else if (node->t == NONTERMINAL && 
            node->gnode.non_term == ITERATIVESTMT){
        if (node->looptype == FOR){
            // ALSO ADD ID TO READONLY FLAG 
            check = checkInSymbolTable(node->child, environment);
            if (check == 0) return 0;
            if (node->child->dtype != _integertype) return 0;
            check = typechecking(node->child->sibling, environment);
            if (check == 0) return 0;
        }
        else {
            check = typechecking(node->child, environment);
            if (check == 0) return 0;
            if (node->child->dtype != _booltype) return 0;
            check = typechecking(node->child->sibling, environment);
            if (check == 0) return 0;
        }
        return 1;
    }
    else if (node->t == NONTERMINAL && 
            node->gnode.non_term == IOSTMT){
        // SEE DIFFERENCE BETWEEN ID AND VAR
        if (node->child->gnode.term == GET_VALUE){
            // ID
            check = checkInSymbolTable(node->child->sibling, environment);
            if (check == 0) return 0; // shilz not present in st
        }
        else {
            check = typechecking(node->child->sibling, environment);
            // check for var
            if (check == 0) return 0;
        }
        return 1;

    }

    // GEN CASE PLUS EXPRESSIONS
    // Declare array of types for each of the child
    //TYPE Ti[numChilds];
    //TYPE Arr[numChilds];
    tempPtr = node->child;
    // RECURSE DOWN
    for (int i = 0; i < numChilds; i++){
        // PASS ENVIRONMENT ACC TO ANY CHANGES IF ANY
        check = typechecking(tempPtr, environment);
        if (check == 0) return 0;
        else{
            //Ti[i] = tempPtr->dtype; // Set Ti to type of child
            //if (Ti[i] == _arrtype)
            //    Arr[i] = tempPtr->arrtype;
        }
        typePtr = typePtr->sibling;
    }
    if (node->gnode.term==PLUS || node->gnode.term==MINUS ||
            node->gnode.term == MUL || node->gnode.term == DIV){
        check = evaluateOperator(node, 'a');
        if (check == 0) return 0;
    }
    else if (node->gnode.term==LT || node->gnode.term==LE ||
            node->gnode.term == GE || node->gnode.term == GT ||
            node->gnode.term == EQ || node->gnode.term == NE){
        check = evaluateOperator(node, 'r');
        if (check == 0) return 0;
    }
    else if (node->gnode.term==AND || node->gnode.term==OR){
        check = evaluateOperator(node, 'l');
        if (check == 0) return 0;
    }

    return 1;
}
