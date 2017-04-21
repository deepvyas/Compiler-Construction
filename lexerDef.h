// Batch Number - 38
// 2014A7PS102P - RISHABH JOSHI
// 2014A7PS248P - DEEP VYAS

#ifndef _LEXERDEF_
#define _LEXERDEF_

#include <stdio.h>

#define LEXEME_SIZE 20
// #define noTerminals 52
typedef enum{
	INTEGER, 
	REAL, 
	BOOLEAN, 
	OF,
	ARRAY,
	START,
	END,
	DECLARE,
	MODULE,
	DRIVER,
	PROGRAM,
	GET_VALUE,
	PRINT,
	USE,
	WITH,
	PARAMETERS,
	TRUE,
	FALSE,
	TAKES,
	INPUT,
	RETURNS,
	AND,
	OR,
	FOR,
	IN,
	SWITCH,
	CASE,
	BREAK,
	DEFAULT,
	WHILE,
	PLUS,
	MINUS,
	MUL,
	DIV,
	LT,
	LE,
	GE,
	GT,
	EQ,
	NE,
	DEF,
	ENDDEF,
	COLON,
	RANGEOP,
	SEMICOL,
	COMMA,
	ASSIGNOP,
	SQBO,
	SQBC,
	BO,
	BC,
	COMMENTMARK,
	ID,
	DRIVERDEF,
	DRIVERENDDEF,
	EPSILON,
	DOLLAR
}TokenType;

typedef union{
	char iden[LEXEME_SIZE];
	int num;
	double r_num;
}lexemeUnion;

typedef struct{
	lexemeUnion lexeme;
	int line_no;
	TokenType token;
	int uniontype;
}tokenInfo;

tokenInfo *getToken(FILE **fp);
FILE* getStream(FILE* fp);
void removeComments(char *testcaseFile);
void printTokenList(char *testcaseFile);


#endif
