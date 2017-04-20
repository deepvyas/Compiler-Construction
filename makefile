all:
	gcc -g ASTDef.h Stage1/hashTable.c Stage1/lexer.c Stage1/parser.c ast.c typeChecking.c typeCheckingDef.h HashTree.c codeGen.c codeGenDef.h -lm
#	gcc ast.c ASTDef.h Stage1/hashTable.c Stage1/lexer.c Stage1/parser.c -lm

gen:
	nasm -felf64 code.asm
	ld code.o -o asm
