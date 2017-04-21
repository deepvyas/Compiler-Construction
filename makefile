## Batch Number - 38
## 2014A7PS102P - RISHABH JOSHI
## 2014A7PS248P - DEEP VYAS

all:
	gcc parser.c ast.c hashTable.c typeChecking.c HashTree.c codeGen.c lexer.c driver.c -o compiler -lm
#	gcc -g ASTDef.h hashTable.c lexer.c parser.c ast.c typeChecking.c typeCheckingDef.h HashTree.c codeGen.c codeGenDef.h -lm
#	gcc ast.c ASTDef.h Stage1/hashTable.c Stage1/lexer.c Stage1/parser.c -lm

gen:
	nasm -f elf64 code.asm
	gcc -m64 -o asm code.o
	#ld code.o -o asm
