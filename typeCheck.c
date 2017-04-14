#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"

#define _booltype 2
#define _arrtype 3

typedef int TYPE

int evlauateTypeArith(ASTNode* node, TYPE Ti[], TYPE Arr[], int numChilds){
    // This function checks if all children have
    // same type for arith to work
    // if That is the case, then put node->dtype or arr type accordingly 
    // as same as Ti
    // return 1 if done
    // else 0
    TYPE t = Ti[0];
    if (t == _arrtype) t = Arr[0];
    for (int i = 1; i < numChilds; i++){
        if (T[i] == _arrtype)
            if (Arr[i] != t) // Array node not of type t
                return 0;
        if (T[i] != t) // Simple node not of type t
            return 0;
    }
    node->dtype = t;
    return 1;
}

int evlauateTypeRel(ASTNode* node, TYPE Ti[], TYPE Arr[], int numChilds){
    // This function checks if all children have
    // same type for relational to work
    // if That is the case, then put node->dtype or arr type accordingly 
    // as boolean
    // return 1 if done
    // else 0
    TYPE t = Ti[0];
    if (t == _arrtype) t = Arr[0];
    for (int i = 1; i < numChilds; i++){
        if (T[i] == _arrtype)
            if (Arr[i] != t) // Array node not of type t
                return 0;
        if (T[i] != t) // Simple node not of type t
            return 0;
    }
    node->dtype = _booltype; // For boolean
    return 1;
}

int evlauateTypelog(ASTNode* node, TYPE Ti[], TYPE Arr[], int numChilds){
    // This function checks if all children have
    // same type for logical to work boolean
    // if That is the case, then put node->dtype or arr type accordingly 
    // as boolean
    // return 1 if done
    // else 0
    TYPE t = Ti[0];
    if (t != _booltype) return 0;
    if (t == _arrtype) t = Arr[0];
    for (int i = 1; i < numChilds; i++){
        if (T[i] == _arrtype)
            if (Arr[i] != t) // Array node not of type t
                return 0;
        if (T[i] != t) // Simple node not of type t
            return 0;
    }
    node->dtype = _booltype; // For boolean
    return 1;
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

    if (numChilds == 0 || node->tokenPtr != NULL){ // CASES HERE
        // Check here for redeclaration
        // Check for present in symbol table
        // Check for scope
        // Check if tokenpointer
        check = checkInSymbolTable(ASTNode *node, environment);
        if (check == 0) return 0; // Some error
        // 4 types of declares handled:
        // NUM then type already integer
        // RNUM then type already real
        // true/flase then type already boolean
        // array type already known too
        return 1;
    }
    // The following will run if node is expression

    // Now numChilds has the number of children of the node
    // Declare array of types for each of the child
    TYPE Ti[numChilds];
    TYPE Arr[numChilds];
    tempPtr = node->child;
    for (int i = 0; i < numChilds; i++){
        // PASS ENVIRONMENT ACC TO ANY CHANGES IF ANY
        check = typechecking(tempPtr, environment);
        if (check == 1){
            Ti[i] = tempPtr->dtype; // Set Ti to type of child
            if (Ti[i] == _arrtype)
                Arr[i] = tempPtr->arrtype;
        }
        else return 0; // return false even if one of the shiz doesnt match
        typePtr = typePtr->sibling;
    }
    if (node->gnode.term==PLUS || node->gnode.term==MINUS ||
            node->gnode.term == MUL || node->gnode.term == DIV){
        check = evaluateTypeArith(node, Ti, Arr, numChilds);
        if (check == 0) return 0;
    }
    else if (node->gnode.term==LT || node->gnode.term==LE ||
            node->gnode.term == GE || node->gnode.term == GT ||
            node->gnode.term == EQ || node->gnode.term == NE){
        check = evaluateTypeRel(node, Ti, Arr, numChilds);
        if (check == 0) return 0;
    }
    else if (node->gnode.term==AND || node->gnode.term==OR){
        check = evaluateTypeArith(node, Ti, Arr, numChilds);
        if (check == 0) return 0;
    }

    return 1;
}
