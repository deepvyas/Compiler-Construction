#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "parserDef.h"
#include "lexerDef.h"
#include "hashTable.h"

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Check usage!  ./a.out testcase.txt parseTree.txt");
        return 0;
    }
    printf("FIRST and FOLLOW set automated\n");
    printf("Both lexical and syntax analysis modules implemented\n");
    printf("modules work with testcases 1, 2, 3 and 4 (All given)\n");
    printf("Parse tree constructed\n");

    initTable();
    populateGrammar();
    compute_first();
    compute_follow();
    createParsingTable();
    // printFollow();
    int task;
    //while(1){
        printf("\nEnter a number corresponding to the tasks : \n");
        printf("    1 - Removal of comments (print on console)\n");
        printf("    2 - For printing token list on console \n");
        printf("    3 - For parsing the verify the syntactic correctness of input source code \n");
        printf("    4 - For creating the parse tree and printing it \n");
        scanf("%d", &task);
        switch(task){
            case 1: removeComments(argv[1]);
                    break;
            case 2: printTokenList(argv[1]);
                    break;
            case 3: parseInputSourceCode(argv[1], 1);
                    break;
            case 4: parseInputSourceCode(argv[1], 0);
                    printParseTree(argv[2]);
                    break;
            default : printf("Incorrect option!\n");
        }
    //}
    return 0;
}
