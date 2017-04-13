all:
	gcc ast.c ASTDef.h Stage1/hashTable.c Stage1/lexer.c Stage1/parser.c -lm
