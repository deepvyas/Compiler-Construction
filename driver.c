#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "parserDef.h"
#include "lexerDef.h"
#include "hashTable.h"
#include "ASTDef.h"
#include "codeGenDef.h"
#include "HashTreeDef.h"
#include "typeCheckingDef.h"

extern FILE *fp;
extern ASTNode *ast_root;


int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Check usage!  ./compiler testcase.txt code.asm");
        return 0;
    }
    int level = 5;
    char message[] = "";
    printf("LEVEL %d: %s", level, message);
    fp = fopen(argv[2], "w");
    
    FILE *stdot = stdout;
    ASTNode *astroot = ast_root;
    int check;
    int task;

    initTable();
    populateGrammar();
    compute_first();
    compute_follow();
    createParsingTable();

    //while(1){
        printf("\nEnter a number corresponding to the tasks : \n");
        printf("    1 - For printing token list on console.\n");
        printf("    2 - For parsing the verify the syntactic correctness of input source code and printing it on console.\n");
        printf("    3 - For printing AST on console. Traversal Order: PreOrder.\n");
        printf("    4 - For displaying the amount of allocated memory and number of nodes in Parse Tree and AST.\n");
        printf("    5 - For printing Symbol Table.\n");
        printf("    6 - For compiling to verify syntactic and semantic correctness of code.\n");
        printf("    7 - For producting assembly code.\n");
        scanf("%d", &task);
        switch(task){
            case 1: printTokenList(argv[1]);
                    break;
            case 2: parseInputSourceCode(argv[1], 0);
                    printParseTree(stdot);
                    break;
            case 3: _printAST(ast_root);
                    break;
            case 4: //printAllocatedMemory(ast_root);
                    break;
            case 5: //printSymbolTable();
                    break;
            case 6: check = typeCheck(argv[1]);
                    if(check == 1)
                        printf("Code compiled successfully....\n");
                    else
                        printf("Errors compiling code....\n");
                    break;
            case 7: if (check)
                        codegen(astroot->child->sibling->sibling);
                    fprintf(fp, "   mov rax, 0\n");
                    fprintf(fp, "   ret\n");
                    char format[] = "db \"%d\", 10, 0";
                    char formatin[] = "db \"%d\", 0";
                    fprintf(fp, "section .data\n");
                    fprintf(fp, "   formatout:  %s\n", format);
                    fprintf(fp, "   formatin:   %s\n", formatin);
                    fprintf(fp, "   location:   times 65536 db 0\n");
                    break;
            default : printf("Incorrect option!\n");
        }
    //}
    return 0;
}
