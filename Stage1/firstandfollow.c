#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parserDef.h"

char nt[51][100] = {
    "PROGRAMNT", ///
    "MODULEDECLARATIONS",	"OTHERMODULES",	"DRIVERMODULE",
    "MODULEDECLARATION",	"MODULENT",////
    "MODULEDEF",	"INPUTPLIST",	"RET",	"OUTPUTPLIST",	"N1",
    "N2",	"DATATYPE",	"RANGE",	"TYPE",	"STATEMENTS",	"STATEMENT",
    "IOSTMT",	"VAR",	"WHICHID",	"SIMPLESTMT",	"ASSIGNMENTSTMT",
    "WHICHSTMT",	"LVALUEIDSTMT",	"LVALUEARRSTMT",	"INDEX",
    "MODULEREUSESTMT",	"OPTIONAL",	"IDLIST",	"N3",	"EXPRESSION",
    "ARITHMETICORBOOLEANEXPR",	"ANYTERM",	"N7",	"N8",
    "ARITHMETICEXPR",	"N4",	"TERM",	"N5",	"FACTOR",	"LOGICALOP",
    "RELATIONALOP",	"OP1",	"OP2",	"DECLARESTMT",	"CONDITIONALSTMT",
	"CASESTMTS",	"N9",	"VALUE",	"DEFAULTNT", /////
	"ITERATIVESTMT"};

char ter[56][100] = {
"INTEGER", 
"REAL", 
"BOOLEAN", 
"OF",
"ARRAY",
"START",
"END",
"DECLARE",
"MODULE",
"DRIVER",
"PROGRAM",
"GET_VALUE",
"PRINT",
"USE",
"WITH",
"PARAMETERS",
"TRUE",
"FALSE",
"TAKES",
"INPUT",
"RETURNS",
"AND",
"OR",
"FOR",
"IN",
"SWITCH",
"CASE",
"BREAK",
"DEFAULT",
"WHILE",
"PLUS",
"MINUS",
"MUL",
"DIV",
"LT",
"LE",
"GE",
"GT",
"EQ",
"NE",
"DEF",
"ENDDEF",
"COLON",
"RANGEOP",
"SEMICOL",
"COMMA",
"ASSIGNOP",
"SQBO",
"SQBC",
"BO",
"BC",
"COMMENTMARK",
"ID",
"DRIVERDEF",
"DRIVERENDDEF",
"EPSILON"
};

FirstAndFollow ffset;

#define size_nt 51
#define sz_t 56


int main(){
	FILE *fp = fopen("follow.txt", "r");

	char *buffer;
	size_t size = 320;
	buffer = (char *)malloc(size * sizeof(char));

	char *token;
	const char s[2] = " ";
	while(fgets(buffer,size,fp)!=NULL){
		// printf("%s",buffer);
		buffer[strlen(buffer)-1] = '\0';
		token = strtok(buffer, s);
		int i = 0;
		if(token==NULL) break;
		for (i=0; i < size_nt;i++){
			if(strcmp(token,nt[i])==0) break;
		}
		int k=0;
		printf("\nFollow of %s : ",nt[i]);
		while(token != NULL){
			token=strtok(NULL, s);
			if(token==NULL) break;
			int j=0;
			for(j=0;j<sz_t;j++){
				if(strcmp(token,ter[j])==0) break;
			}
			ffset.follow[i][k++].t=j;
			printf("%s ,",ter[j]);
		}
		ffset.follow[i][k++].grammarindex=-1;
		printf("\n");
	}
	return 0;
}
