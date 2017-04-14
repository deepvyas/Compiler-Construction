#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"

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
    // Declare array of types for each of the child
    TYPE Ti[numChilds];
    tempPtr = node->child;
    for (int i = 0; i < numChilds; i++){
        // PASS ENVIRONMENT ACC TO ANY CHANGES IF ANY
        check = typechecking(tempPtr, environment);
        if (check == 1){
            Ti[i] = tempPtr->dtype; // Set Ti to type of child
        }
        else return 0; // return false even if one of the shiz doesnt match
        typePtr = typePtr->sibling;
    }

    return T;
}
