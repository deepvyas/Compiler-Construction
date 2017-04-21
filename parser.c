// Batch Number - 38
// 2014A7PS102P - RISHABH JOSHI
// 2014A7PS248P - DEEP VYAS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

#include "parserDef.h"
#include "lexerDef.h"
#include "hashTable.h"

// Gammar struct
GrammarNode grammar[GRAMMAR_LEN][GRAMMAR_LEN];

// Global parsing table
int parsingTable[noNonTerminals][noTerminals];

// ffset is the first or follow set
FirstAndFollow ffset;

char nt[noNonTerminals][100] = {
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
	"ITERATIVESTMT", "NEGVAR", "XXX"};

char ter[noTerminals][100] = {
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
	"EPSILON",
	"EOF"
};

FirstAndFollow ffset;

int first[noNonTerminals][noTerminals];
int follow[noNonTerminals][noTerminals];
int follow_vis[noNonTerminals];

#define size_nt 51
#define sz_t 57

void populateFirstSet(){
	FILE *fp = fopen("first.txt", "r");

	char *buffer;
	size_t size = 132;
	buffer = (char *)malloc(size * sizeof(char));

	char *token;
	const char s[2] = " ";
	while(fgets(buffer, size, fp)!= NULL){
		buffer[strlen(buffer)-1] = '\0';
		token = strtok(buffer, s);
		int i = 0;
		if (token == NULL)
			break;
		for (i = 0; i < size_nt; i++){
			if (strcmp(token, nt[i]) == 0)
				break;
		}
        // printf("\nfirst of %s : ", nt[i]);
		int k = 0;
        // i is of index into ffset.first[i][j++].t = nexttokenvalue
		while(token != NULL){   
			token = strtok(NULL, s);
        //    printf("-%s-", token);
        //    fflush(stdout);
			if(token == NULL) break;
			int j = 0;
			for (j = 0; j < sz_t; j++){
				if (strcmp(token, ter[j]) == 0)
					break;
			}
			ffset.first[i][k++].t = j;
            // printf("%s ,", t[j]);
		}
		ffset.first[i][k++].grammarindex = -1;
	}
}

void populateFollowSet(){
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
		// printf("\nFollow of %s : ",nt[i]);
		while(token != NULL){
			token=strtok(NULL, s);
			if(token==NULL) break;
			int j=0;
			for(j=0;j<sz_t;j++){
				if(strcmp(token,ter[j])==0) break;
			}
			if(strcmp(token,"EOF")==0) ffset.follow[i][k++].t=60;
			else ffset.follow[i][k++].t=j;
			// printf("%s ,",ter[j]);
		}
		ffset.follow[i][k++].grammarindex=-1;
		// printf("\n");
	}
}

void populateGrammar(){
	grammar[0][0].ele.non_term = PROGRAMNT;
	grammar[0][0].t = NONTERMINAL;
	grammar[0][0].len = 4;
	grammar[0][1].ele.non_term = MODULEDECLARATIONS;
	grammar[0][1].t = NONTERMINAL;
	grammar[0][2].ele.non_term = OTHERMODULES;
	grammar[0][2].t = NONTERMINAL;
	grammar[0][3].ele.non_term = DRIVERMODULE;
	grammar[0][3].t = NONTERMINAL;
	grammar[0][4].ele.non_term = OTHERMODULES;
	grammar[0][4].t = NONTERMINAL;

	grammar[1][0].ele.non_term = MODULEDECLARATIONS;
	grammar[1][0].t = NONTERMINAL;
	grammar[1][0].len = 2;
	grammar[1][1].ele.non_term = MODULEDECLARATION;
	grammar[1][1].t = NONTERMINAL;
	grammar[1][2].ele.non_term = MODULEDECLARATIONS;
	grammar[1][2].t = NONTERMINAL;

  	
  grammar[2][0].ele.non_term = MODULEDECLARATIONS;
	grammar[2][0].t = NONTERMINAL;
	grammar[2][0].len = 1;
	grammar[2][1].ele.term = EPSILON;
	grammar[2][1].t = TERMINAL;
	
	grammar[3][0].ele.non_term = MODULEDECLARATION;
	grammar[3][0].t = NONTERMINAL;
	grammar[3][0].len = 4;
	grammar[3][1].ele.term = DECLARE;
	grammar[3][1].t = TERMINAL;
	grammar[3][2].ele.term = MODULE;
	grammar[3][2].t = TERMINAL;
	grammar[3][3].ele.term = ID;
	grammar[3][3].t = TERMINAL;
	grammar[3][4].ele.term = SEMICOL;
	grammar[3][4].t = TERMINAL;

	grammar[4][0].ele.non_term = DRIVERMODULE;
	grammar[4][0].t = NONTERMINAL;
	grammar[4][0].len = 5;
	grammar[4][1].ele.term = DRIVERDEF;
	grammar[4][1].t = TERMINAL;
	grammar[4][2].ele.term = DRIVER;
	grammar[4][2].t = TERMINAL;
	grammar[4][3].ele.term = PROGRAM;
	grammar[4][3].t = TERMINAL;
	grammar[4][4].ele.term = DRIVERENDDEF;
	grammar[4][4].t = TERMINAL;
	grammar[4][5].ele.non_term = MODULEDEF;
	grammar[4][5].t = NONTERMINAL;

	grammar[5][0].ele.non_term = MODULENT;
	grammar[5][0].t = NONTERMINAL;
	grammar[5][0].len = 12;
	grammar[5][1].ele.term = DEF;
	grammar[5][1].t = TERMINAL;
	grammar[5][2].ele.term = MODULE;
	grammar[5][2].t = TERMINAL;
	grammar[5][3].ele.term = ID;
	grammar[5][3].t = TERMINAL;
	grammar[5][4].ele.term = ENDDEF;
	grammar[5][4].t = TERMINAL;
	grammar[5][5].ele.term = TAKES;
	grammar[5][5].t = TERMINAL;
	grammar[5][6].ele.term = INPUT;
	grammar[5][6].t = TERMINAL;
	grammar[5][7].ele.term = SQBO;
	grammar[5][7].t = TERMINAL;
	grammar[5][8].ele.non_term = INPUTPLIST;
	grammar[5][8].t = NONTERMINAL;
	grammar[5][9].ele.term = SQBC;
	grammar[5][9].t = TERMINAL;
	grammar[5][10].ele.term = SEMICOL;
	grammar[5][10].t = TERMINAL;
	grammar[5][11].ele.non_term = RET;
	grammar[5][11].t = NONTERMINAL;
	grammar[5][12].ele.non_term = MODULEDEF;
	grammar[5][12].t = NONTERMINAL;

	grammar[6][0].ele.non_term = RET;
	grammar[6][0].t = NONTERMINAL;
	grammar[6][0].len = 5;
	grammar[6][1].ele.term = RETURNS;
	grammar[6][1].t = TERMINAL;
	grammar[6][2].ele.term = SQBO;
	grammar[6][2].t = TERMINAL;
	grammar[6][3].ele.non_term = OUTPUTPLIST;
	grammar[6][3].t = NONTERMINAL;
	grammar[6][4].ele.term = SQBC;
	grammar[6][4].t = TERMINAL;
	grammar[6][5].ele.term = SEMICOL;
	grammar[6][5].t = TERMINAL;

	grammar[7][0].ele.non_term = RET;
	grammar[7][0].t = NONTERMINAL;
	grammar[7][0].len = 1;
	grammar[7][1].ele.term = EPSILON;
	grammar[7][1].t = TERMINAL;
	
	grammar[8][0].ele.non_term = INPUTPLIST;
	grammar[8][0].t = NONTERMINAL;
	grammar[8][0].len = 4;
	grammar[8][1].ele.term = ID;
	grammar[8][1].t = TERMINAL;
	grammar[8][2].ele.term = COLON;
	grammar[8][2].t = TERMINAL;
	grammar[8][3].ele.non_term = DATATYPE;
	grammar[8][3].t = NONTERMINAL;
	grammar[8][4].ele.non_term = N1;
	grammar[8][4].t = NONTERMINAL;

	grammar[9][0].ele.non_term = N1;
	grammar[9][0].t = NONTERMINAL;
	grammar[9][0].len = 5;
	grammar[9][1].ele.term = COMMA;
	grammar[9][1].t = TERMINAL;
	grammar[9][2].ele.term = ID;
	grammar[9][2].t = TERMINAL;
	grammar[9][3].ele.term = COLON;
	grammar[9][3].t = TERMINAL;
	grammar[9][4].ele.non_term = DATATYPE;
	grammar[9][4].t = NONTERMINAL;
	grammar[9][5].ele.non_term = N1;
	grammar[9][5].t = NONTERMINAL;

	grammar[10][0].ele.non_term = N1;
	grammar[10][0].t = NONTERMINAL;
	grammar[10][0].len = 1;
	grammar[10][1].ele.term = EPSILON;
	grammar[10][1].t = TERMINAL;

	grammar[11][0].ele.non_term = OUTPUTPLIST;
	grammar[11][0].t = NONTERMINAL;
	grammar[11][0].len = 4;
	grammar[11][1].ele.term = ID;
	grammar[11][1].t = TERMINAL;
	grammar[11][2].ele.term = COLON;
	grammar[11][2].t = TERMINAL;
	grammar[11][3].ele.non_term = TYPE;
	grammar[11][3].t = NONTERMINAL;
	grammar[11][4].ele.non_term = N2;
	grammar[11][4].t = NONTERMINAL;

	grammar[12][0].ele.non_term = N2;
	grammar[12][0].t = NONTERMINAL;
	grammar[12][0].len = 5;
	grammar[12][1].ele.term = COMMA;
	grammar[12][1].t = TERMINAL;
	grammar[12][2].ele.term = ID;
	grammar[12][2].t = TERMINAL;
	grammar[12][3].ele.term = COLON;
	grammar[12][3].t = TERMINAL;
	grammar[12][4].ele.non_term = TYPE;
	grammar[12][4].t = NONTERMINAL;
	grammar[12][5].ele.non_term = N2;
	grammar[12][5].t = NONTERMINAL;

	grammar[13][0].ele.non_term = N2;
	grammar[13][0].t = NONTERMINAL;
	grammar[13][0].len = 1;
	grammar[13][1].ele.term = EPSILON;
	grammar[13][1].t = TERMINAL;

	grammar[14][0].ele.non_term = DATATYPE;
	grammar[14][0].t = NONTERMINAL;
	grammar[14][0].len = 1;
	grammar[14][1].ele.term = INTEGER;
	grammar[14][1].t = TERMINAL;

	grammar[15][0].ele.non_term = DATATYPE;
	grammar[15][0].t = NONTERMINAL;
	grammar[15][0].len = 1;
	grammar[15][1].ele.term = REAL;
	grammar[15][1].t = TERMINAL;

	grammar[16][0].ele.non_term = DATATYPE;
	grammar[16][0].t = NONTERMINAL;
	grammar[16][0].len = 1;
	grammar[16][1].ele.term = BOOLEAN;
	grammar[16][1].t = TERMINAL;

	grammar[17][0].ele.non_term = DATATYPE;
	grammar[17][0].t = NONTERMINAL;
	grammar[17][0].len = 6;
	grammar[17][1].ele.term = ARRAY;
	grammar[17][1].t = TERMINAL;
	grammar[17][2].ele.term = SQBO;
	grammar[17][2].t = TERMINAL;
	grammar[17][3].ele.non_term = RANGE;
	grammar[17][3].t = NONTERMINAL;
	grammar[17][4].ele.term = SQBC;
	grammar[17][4].t = TERMINAL;
	grammar[17][5].ele.term = OF;
	grammar[17][5].t = TERMINAL;
	grammar[17][6].ele.non_term = TYPE;
	grammar[17][6].t = NONTERMINAL;

	grammar[18][0].ele.non_term = TYPE;
	grammar[18][0].t = NONTERMINAL;
	grammar[18][0].len = 1;
	grammar[18][1].ele.term = INTEGER;
	grammar[18][1].t = TERMINAL;

	grammar[19][0].ele.non_term = TYPE;
	grammar[19][0].t = NONTERMINAL;
	grammar[19][0].len = 1;
	grammar[19][1].ele.term = REAL;
	grammar[19][1].t = TERMINAL;

	grammar[20][0].ele.non_term = TYPE;
	grammar[20][0].t = NONTERMINAL;
	grammar[20][0].len = 1;
	grammar[20][1].ele.term = BOOLEAN;
	grammar[20][1].t = TERMINAL;

	grammar[21][0].ele.non_term = MODULEDEF;
	grammar[21][0].t = NONTERMINAL;
	grammar[21][0].len = 3;
	grammar[21][1].ele.term = START;
	grammar[21][1].t = TERMINAL;
	grammar[21][2].ele.non_term = STATEMENTS;
	grammar[21][2].t = NONTERMINAL;
	grammar[21][3].ele.term = END;
	grammar[21][3].t = TERMINAL;

	grammar[22][0].ele.non_term = STATEMENTS;
	grammar[22][0].t = NONTERMINAL;
	grammar[22][0].len = 2;
	grammar[22][1].ele.non_term = STATEMENT;
	grammar[22][1].t = NONTERMINAL;
	grammar[22][2].ele.non_term = STATEMENTS;
	grammar[22][2].t = NONTERMINAL;
	
	grammar[23][0].ele.non_term = STATEMENTS;
	grammar[23][0].t = NONTERMINAL;
	grammar[23][0].len = 1;
	grammar[23][1].ele.term = EPSILON;
	grammar[23][1].t = TERMINAL;	

	grammar[24][0].ele.non_term = STATEMENT;
	grammar[24][0].t = NONTERMINAL;
	grammar[24][0].len = 1;
	grammar[24][1].ele.non_term = IOSTMT;
	grammar[24][1].t = NONTERMINAL;

	grammar[25][0].ele.non_term = STATEMENT;
	grammar[25][0].t = NONTERMINAL;
	grammar[25][0].len = 1;
	grammar[25][1].ele.non_term = SIMPLESTMT;
	grammar[25][1].t = NONTERMINAL;

	grammar[26][0].ele.non_term = STATEMENT;
	grammar[26][0].t = NONTERMINAL;
	grammar[26][0].len = 1;
	grammar[26][1].ele.non_term = DECLARESTMT;
	grammar[26][1].t = NONTERMINAL;

	grammar[27][0].ele.non_term = STATEMENT;
	grammar[27][0].t = NONTERMINAL;
	grammar[27][0].len = 1;
	grammar[27][1].ele.non_term = CONDITIONALSTMT;
	grammar[27][1].t = NONTERMINAL;

	grammar[28][0].ele.non_term = STATEMENT;
	grammar[28][0].t = NONTERMINAL;
	grammar[28][0].len = 1;
	grammar[28][1].ele.non_term = ITERATIVESTMT;
	grammar[28][1].t = NONTERMINAL;

	grammar[29][0].ele.non_term = IOSTMT;
	grammar[29][0].t = NONTERMINAL;
	grammar[29][0].len = 5;
	grammar[29][1].ele.term = GET_VALUE;
	grammar[29][1].t = TERMINAL;
	grammar[29][2].ele.term = BO;
	grammar[29][2].t = TERMINAL;
	grammar[29][3].ele.term = ID;
	grammar[29][3].t = TERMINAL;
	grammar[29][4].ele.term = BC;
	grammar[29][4].t = TERMINAL;
	grammar[29][5].ele.term = SEMICOL;
	grammar[29][5].t = TERMINAL;

	grammar[30][0].ele.non_term = IOSTMT;
	grammar[30][0].t = NONTERMINAL;
	grammar[30][0].len = 5;
	grammar[30][1].ele.term = PRINT;
	grammar[30][1].t = TERMINAL;
	grammar[30][2].ele.term = BO;
	grammar[30][2].t = TERMINAL;
	grammar[30][3].ele.non_term = VAR;
	grammar[30][3].t = NONTERMINAL;
	grammar[30][4].ele.term = BC;
	grammar[30][4].t = TERMINAL;
	grammar[30][5].ele.term = SEMICOL;
	grammar[30][5].t = TERMINAL;

	grammar[31][0].ele.non_term = VAR;
	grammar[31][0].t = NONTERMINAL;
	grammar[31][0].len = 1;
	grammar[31][1].ele.term = INTEGER;
	grammar[31][1].t = TERMINAL;	

	grammar[32][0].ele.non_term = VAR;
	grammar[32][0].t = NONTERMINAL;
	grammar[32][0].len = 1;
	grammar[32][1].ele.term = REAL;
	grammar[32][1].t = TERMINAL;

	grammar[33][0].ele.non_term = VAR;
	grammar[33][0].t = NONTERMINAL;
	grammar[33][0].len = 2;
	grammar[33][1].ele.term = ID;
	grammar[33][1].t = TERMINAL;
	grammar[33][2].ele.non_term = WHICHID;
	grammar[33][2].t = NONTERMINAL;

  // Rest of var and negvar below
	
	grammar[34][0].ele.non_term = WHICHID;
	grammar[34][0].t = NONTERMINAL;
	grammar[34][0].len = 1;
	grammar[34][1].ele.term = EPSILON;
	grammar[34][1].t = TERMINAL;

	grammar[35][0].ele.non_term = WHICHID;
	grammar[35][0].t = NONTERMINAL;
	grammar[35][0].len = 3;
	grammar[35][1].ele.term = SQBO;
	grammar[35][1].t = TERMINAL;
	grammar[35][2].ele.term = ID;
	grammar[35][2].t = TERMINAL;
	grammar[35][3].ele.term = SQBC;
	grammar[35][3].t = TERMINAL;

	grammar[36][0].ele.non_term = SIMPLESTMT;
	grammar[36][0].t = NONTERMINAL;
	grammar[36][0].len = 1;
	grammar[36][1].ele.non_term = ASSIGNMENTSTMT;
	grammar[36][1].t = NONTERMINAL;

	grammar[37][0].ele.non_term = SIMPLESTMT;
	grammar[37][0].t = NONTERMINAL;
	grammar[37][0].len = 1;
	grammar[37][1].ele.non_term = MODULEREUSESTMT;
	grammar[37][1].t = NONTERMINAL;

	grammar[38][0].ele.non_term = ASSIGNMENTSTMT;
	grammar[38][0].t = NONTERMINAL;
	grammar[38][0].len = 2;
	grammar[38][1].ele.term = ID;
	grammar[38][1].t = TERMINAL;
	grammar[38][2].ele.non_term = WHICHSTMT;
	grammar[38][2].t = NONTERMINAL;

	grammar[39][0].ele.non_term = WHICHSTMT;
	grammar[39][0].t = NONTERMINAL;
	grammar[39][0].len = 1;
	grammar[39][1].ele.non_term = LVALUEIDSTMT;
	grammar[39][1].t = NONTERMINAL;

	grammar[40][0].ele.non_term = WHICHSTMT;
	grammar[40][0].t = NONTERMINAL;
	grammar[40][0].len = 1;
	grammar[40][1].ele.non_term = LVALUEARRSTMT;
	grammar[40][1].t = NONTERMINAL;

	grammar[41][0].ele.non_term = LVALUEIDSTMT;
	grammar[41][0].t = NONTERMINAL;
	grammar[41][0].len = 3;
	grammar[41][1].ele.term = ASSIGNOP;
	grammar[41][1].t = TERMINAL;
	grammar[41][2].ele.non_term = EXPRESSION;
	grammar[41][2].t = NONTERMINAL;
	grammar[41][3].ele.term = SEMICOL;
	grammar[41][3].t = TERMINAL;

	grammar[42][0].ele.non_term = LVALUEARRSTMT;
	grammar[42][0].t = NONTERMINAL;
	grammar[42][0].len = 6;
	grammar[42][1].ele.term = SQBO;
	grammar[42][1].t = TERMINAL;
	grammar[42][2].ele.non_term = INDEX;
	grammar[42][2].t = NONTERMINAL;
	grammar[42][3].ele.term = SQBC;
	grammar[42][3].t = TERMINAL;
	grammar[42][4].ele.term = ASSIGNOP;
	grammar[42][4].t = TERMINAL;
	grammar[42][5].ele.non_term = EXPRESSION;
	grammar[42][5].t = NONTERMINAL;
	grammar[42][6].ele.term = SEMICOL;
	grammar[42][6].t = TERMINAL;

	grammar[43][0].ele.non_term = INDEX;
	grammar[43][0].t = NONTERMINAL;
	grammar[43][0].len = 1;
	grammar[43][1].ele.term = INTEGER;
	grammar[43][1].t = TERMINAL;

	grammar[44][0].ele.non_term = INDEX;
	grammar[44][0].t = NONTERMINAL;
	grammar[44][0].len = 1;
	grammar[44][1].ele.term = ID;
	grammar[44][1].t = TERMINAL;

	grammar[45][0].ele.non_term = MODULEREUSESTMT;
	grammar[45][0].t = NONTERMINAL;
	grammar[45][0].len = 8;
	grammar[45][1].ele.non_term = OPTIONAL;
	grammar[45][1].t = NONTERMINAL;
	grammar[45][2].ele.term = USE;
	grammar[45][2].t = TERMINAL;
	grammar[45][3].ele.term = MODULE;
	grammar[45][3].t = TERMINAL;
	grammar[45][4].ele.term = ID;
	grammar[45][4].t = TERMINAL;
	grammar[45][5].ele.term = WITH;
	grammar[45][5].t = TERMINAL;
	grammar[45][6].ele.term = PARAMETERS;
	grammar[45][6].t = TERMINAL;
	grammar[45][7].ele.non_term = IDLIST;
	grammar[45][7].t = NONTERMINAL;
	grammar[45][8].ele.term = SEMICOL;
	grammar[45][8].t = TERMINAL;

	grammar[46][0].ele.non_term = OPTIONAL;
	grammar[46][0].t = NONTERMINAL;
	grammar[46][0].len = 1;
	grammar[46][1].ele.term = EPSILON;
	grammar[46][1].t = TERMINAL;

	grammar[47][0].ele.non_term = OPTIONAL;
	grammar[47][0].t = NONTERMINAL;
	grammar[47][0].len = 4;
	grammar[47][1].ele.term = SQBO;
	grammar[47][1].t = TERMINAL;
	grammar[47][2].ele.non_term = IDLIST;
	grammar[47][2].t = NONTERMINAL;
	grammar[47][3].ele.term = SQBC;
	grammar[47][3].t = TERMINAL;
	grammar[47][4].ele.term = ASSIGNOP;
	grammar[47][4].t = TERMINAL;
	
	grammar[48][0].ele.non_term = IDLIST;
	grammar[48][0].t = NONTERMINAL;
	grammar[48][0].len = 2;
	grammar[48][1].ele.term = ID;
	grammar[48][1].t = TERMINAL;
	grammar[48][2].ele.non_term = N3;
	grammar[48][2].t = NONTERMINAL;

	grammar[49][0].ele.non_term = N3;
	grammar[49][0].t = NONTERMINAL;
	grammar[49][0].len = 1;
	grammar[49][1].ele.term = EPSILON;
	grammar[49][1].t = TERMINAL;	

	grammar[50][0].ele.non_term = N3;
	grammar[50][0].t = NONTERMINAL;
	grammar[50][0].len = 3;
	grammar[50][1].ele.term = COMMA;
	grammar[50][1].t = TERMINAL;
	grammar[50][2].ele.term = ID;
	grammar[50][2].t = TERMINAL;
	grammar[50][3].ele.non_term = N3;
	grammar[50][3].t = NONTERMINAL;
	
	grammar[51][0].ele.non_term = EXPRESSION;
	grammar[51][0].t = NONTERMINAL;
	grammar[51][0].len = 1;
	grammar[51][1].ele.non_term = ARITHMETICORBOOLEANEXPR;
	grammar[51][1].t = NONTERMINAL;

	// grammar[52][0].ele.non_term = EXPRESSION;
	// grammar[52][0].t = NONTERMINAL;
	// grammar[52][0].len = 4;
	// grammar[52][1].ele.term = MINUS;
	// grammar[52][1].t = TERMINAL;
	// grammar[52][2].ele.term = BO;
	// grammar[52][2].t = TERMINAL;
	// grammar[52][3].ele.non_term = ARITHMETICEXPR;
	// grammar[52][3].t = NONTERMINAL;
	// grammar[52][4].ele.term = BC;
	// grammar[52][4].t = TERMINAL;

	grammar[52][0].ele.non_term = XXX;
	grammar[52][0].t = NONTERMINAL;
	grammar[52][0].len = 1;
	grammar[52][1].ele.non_term = VAR;
	grammar[52][1].t = NONTERMINAL;
	
	grammar[53][0].ele.non_term = ARITHMETICORBOOLEANEXPR;
	grammar[53][0].t = NONTERMINAL;
	grammar[53][0].len = 2;
	grammar[53][1].ele.non_term = ANYTERM;
	grammar[53][1].t = NONTERMINAL;
	grammar[53][2].ele.non_term = N7;
	grammar[53][2].t = NONTERMINAL;
	
	grammar[54][0].ele.non_term = N7;
	grammar[54][0].t = NONTERMINAL;
	grammar[54][0].len = 1;
	grammar[54][1].ele.term = EPSILON;
	grammar[54][1].t = TERMINAL;	

	grammar[55][0].ele.non_term = N7;
	grammar[55][0].t = NONTERMINAL;
	grammar[55][0].len = 3;
	grammar[55][1].ele.non_term = LOGICALOP;
	grammar[55][1].t = NONTERMINAL;
	grammar[55][2].ele.non_term = ANYTERM;
	grammar[55][2].t = NONTERMINAL;
	grammar[55][3].ele.non_term = N7;
	grammar[55][3].t = NONTERMINAL;
	
	grammar[56][0].ele.non_term = ANYTERM;
	grammar[56][0].t = NONTERMINAL;
	grammar[56][0].len = 2;
	grammar[56][1].ele.non_term = ARITHMETICEXPR;
	grammar[56][1].t = NONTERMINAL;
	grammar[56][2].ele.non_term = N8;
	grammar[56][2].t = NONTERMINAL;
	
	grammar[57][0].ele.non_term = N8;
	grammar[57][0].t = NONTERMINAL;
	grammar[57][0].len = 1;
	grammar[57][1].ele.term = EPSILON;
	grammar[57][1].t = TERMINAL;	

	grammar[58][0].ele.non_term = N8;
	grammar[58][0].t = NONTERMINAL;
	grammar[58][0].len = 3;
	grammar[58][1].ele.non_term = RELATIONALOP;
	grammar[58][1].t = NONTERMINAL;
	grammar[58][2].ele.non_term = ARITHMETICEXPR;
	grammar[58][2].t = NONTERMINAL;
	grammar[58][3].ele.non_term = N8;
	grammar[58][3].t = NONTERMINAL;

	grammar[59][0].ele.non_term = ARITHMETICEXPR;
	grammar[59][0].t = NONTERMINAL;
	grammar[59][0].len = 2;
	grammar[59][1].ele.non_term = TERM;
	grammar[59][1].t = NONTERMINAL;
	grammar[59][2].ele.non_term = N4;
	grammar[59][2].t = NONTERMINAL;

	grammar[60][0].ele.non_term = N4;
	grammar[60][0].t = NONTERMINAL;
	grammar[60][0].len = 1;
	grammar[60][1].ele.term = EPSILON;
	grammar[60][1].t = TERMINAL;	

	grammar[61][0].ele.non_term = N4;
	grammar[61][0].t = NONTERMINAL;
	grammar[61][0].len = 3;
	grammar[61][1].ele.non_term = OP1;
	grammar[61][1].t = NONTERMINAL;
	grammar[61][2].ele.non_term = TERM;
	grammar[61][2].t = NONTERMINAL;
	grammar[61][3].ele.non_term = N4;
	grammar[61][3].t = NONTERMINAL;	
	
	grammar[62][0].ele.non_term = TERM;
	grammar[62][0].t = NONTERMINAL;
	grammar[62][0].len = 2;
	grammar[62][1].ele.non_term = FACTOR;
	grammar[62][1].t = NONTERMINAL;
	grammar[62][2].ele.non_term = N5;
	grammar[62][2].t = NONTERMINAL;

	grammar[63][0].ele.non_term = N5;
	grammar[63][0].t = NONTERMINAL;
	grammar[63][0].len = 1;
	grammar[63][1].ele.term = EPSILON;
	grammar[63][1].t = TERMINAL;	

	grammar[64][0].ele.non_term = N5;
	grammar[64][0].t = NONTERMINAL;
	grammar[64][0].len = 3;
	grammar[64][1].ele.non_term = OP2;
	grammar[64][1].t = NONTERMINAL;
	grammar[64][2].ele.non_term = FACTOR;
	grammar[64][2].t = NONTERMINAL;
	grammar[64][3].ele.non_term = N5;
	grammar[64][3].t = NONTERMINAL;	
	
	grammar[65][0].ele.non_term = FACTOR;
	grammar[65][0].t = NONTERMINAL;
	grammar[65][0].len = 2;
	grammar[65][1].t = TERMINAL;
	grammar[65][1].ele.term = MINUS;
	grammar[65][2].ele.non_term = XXX;
	grammar[65][2].t = NONTERMINAL;	

	grammar[66][0].ele.non_term = OP1;
	grammar[66][0].t = NONTERMINAL;
	grammar[66][0].len = 1;
	grammar[66][1].ele.term = PLUS;
	grammar[66][1].t = TERMINAL;

	grammar[67][0].ele.non_term = OP1;
	grammar[67][0].t = NONTERMINAL;
	grammar[67][0].len = 1;
	grammar[67][1].ele.term = MINUS;
	grammar[67][1].t = TERMINAL;

	grammar[68][0].ele.non_term = OP2;
	grammar[68][0].t = NONTERMINAL;
	grammar[68][0].len = 1;
	grammar[68][1].ele.term = MUL;
	grammar[68][1].t = TERMINAL;

	grammar[69][0].ele.non_term = OP2;
	grammar[69][0].t = NONTERMINAL;
	grammar[69][0].len = 1;
	grammar[69][1].ele.term = DIV;
	grammar[69][1].t = TERMINAL;

	grammar[70][0].ele.non_term = LOGICALOP;
	grammar[70][0].t = NONTERMINAL;
	grammar[70][0].len = 1;
	grammar[70][1].ele.term = AND;
	grammar[70][1].t = TERMINAL;

	grammar[71][0].ele.non_term = LOGICALOP;
	grammar[71][0].t = NONTERMINAL;
	grammar[71][0].len = 1;
	grammar[71][1].ele.term = OR;
	grammar[71][1].t = TERMINAL;

	grammar[72][0].ele.non_term = RELATIONALOP;
	grammar[72][0].t = NONTERMINAL;
	grammar[72][0].len = 1;
	grammar[72][1].ele.term = LT;
	grammar[72][1].t = TERMINAL;

	grammar[73][0].ele.non_term = RELATIONALOP;
	grammar[73][0].t = NONTERMINAL;
	grammar[73][0].len = 1;
	grammar[73][1].ele.term = LE;
	grammar[73][1].t = TERMINAL;
	
	grammar[74][0].ele.non_term = RELATIONALOP;
	grammar[74][0].t = NONTERMINAL;
	grammar[74][0].len = 1;
	grammar[74][1].ele.term = GT;
	grammar[74][1].t = TERMINAL;

	grammar[75][0].ele.non_term = RELATIONALOP;
	grammar[75][0].t = NONTERMINAL;
	grammar[75][0].len = 1;
	grammar[75][1].ele.term = GE;
	grammar[75][1].t = TERMINAL;
	
	grammar[76][0].ele.non_term = RELATIONALOP;
	grammar[76][0].t = NONTERMINAL;
	grammar[76][0].len = 1;
	grammar[76][1].ele.term = EQ;
	grammar[76][1].t = TERMINAL;
	
	grammar[77][0].ele.non_term = RELATIONALOP;
	grammar[77][0].t = NONTERMINAL;
	grammar[77][0].len = 1;
	grammar[77][1].ele.term = NE;
	grammar[77][1].t = TERMINAL;

	grammar[78][0].ele.non_term = DECLARESTMT;
	grammar[78][0].t = NONTERMINAL;
	grammar[78][0].len = 5;
	grammar[78][1].ele.term = DECLARE;
	grammar[78][1].t = TERMINAL;
	grammar[78][2].ele.non_term = IDLIST;
	grammar[78][2].t = NONTERMINAL;
	grammar[78][3].ele.term = COLON;
	grammar[78][3].t = TERMINAL;
	grammar[78][4].ele.non_term = DATATYPE;
	grammar[78][4].t = NONTERMINAL;
	grammar[78][5].ele.term = SEMICOL;
	grammar[78][5].t = TERMINAL;
	
	grammar[79][0].ele.non_term = CONDITIONALSTMT;
	grammar[79][0].t = NONTERMINAL;
	grammar[79][0].len = 8;
	grammar[79][1].ele.term = SWITCH;
	grammar[79][1].t = TERMINAL;
	grammar[79][2].ele.term = BO;
	grammar[79][2].t = TERMINAL;
	grammar[79][3].ele.term = ID;
	grammar[79][3].t = TERMINAL;
	grammar[79][4].ele.term = BC;
	grammar[79][4].t = TERMINAL;
	grammar[79][5].ele.term = START;
	grammar[79][5].t = TERMINAL;
	grammar[79][6].ele.non_term = CASESTMTS;
	grammar[79][6].t = NONTERMINAL;
	grammar[79][7].ele.non_term = DEFAULTNT;
	grammar[79][7].t = NONTERMINAL;
	grammar[79][8].ele.term = END;
	grammar[79][8].t = TERMINAL;

	grammar[80][0].ele.non_term = CASESTMTS;
	grammar[80][0].t = NONTERMINAL;
	grammar[80][0].len = 7;
	grammar[80][1].ele.term = CASE;
	grammar[80][1].t = TERMINAL;
	grammar[80][2].ele.non_term = VALUE;
	grammar[80][2].t = NONTERMINAL;
	grammar[80][3].ele.term = COLON;
	grammar[80][3].t = TERMINAL;
	grammar[80][4].ele.non_term = STATEMENTS;
	grammar[80][4].t = NONTERMINAL;
	grammar[80][5].ele.term = BREAK;
	grammar[80][5].t = TERMINAL;
	grammar[80][6].ele.term = SEMICOL;
	grammar[80][6].t = TERMINAL;
	grammar[80][7].ele.non_term = N9;
	grammar[80][7].t = NONTERMINAL;

	grammar[81][0].ele.non_term = N9;
	grammar[81][0].t = NONTERMINAL;
	grammar[81][0].len = 7;
	grammar[81][1].ele.term = CASE;
	grammar[81][1].t = TERMINAL;
	grammar[81][2].ele.non_term = VALUE;
	grammar[81][2].t = NONTERMINAL;
	grammar[81][3].ele.term = COLON;
	grammar[81][3].t = TERMINAL;
	grammar[81][4].ele.non_term = STATEMENTS;
	grammar[81][4].t = NONTERMINAL;
	grammar[81][5].ele.term = BREAK;
	grammar[81][5].t = TERMINAL;
	grammar[81][6].ele.term = SEMICOL;
	grammar[81][6].t = TERMINAL;
	grammar[81][7].ele.non_term = N9;
	grammar[81][7].t = NONTERMINAL;
	
	grammar[82][0].ele.non_term = N9;
	grammar[82][0].t = NONTERMINAL;
	grammar[82][0].len = 1;
	grammar[82][1].ele.term = EPSILON;
	grammar[82][1].t = TERMINAL;	

	grammar[83][0].ele.non_term = VALUE;
	grammar[83][0].t = NONTERMINAL;
	grammar[83][0].len = 1;
	grammar[83][1].ele.term = INTEGER;
	grammar[83][1].t = TERMINAL;	

	grammar[84][0].ele.non_term = VALUE;
	grammar[84][0].t = NONTERMINAL;
	grammar[84][0].len = 1;
	grammar[84][1].ele.term = TRUE;
	grammar[84][1].t = TERMINAL;	

	grammar[85][0].ele.non_term = VALUE;
	grammar[85][0].t = NONTERMINAL;
	grammar[85][0].len = 1;
	grammar[85][1].ele.term = FALSE;
	grammar[85][1].t = TERMINAL;	

	grammar[86][0].ele.non_term = DEFAULTNT;
	grammar[86][0].t = NONTERMINAL;
	grammar[86][0].len = 5;
	grammar[86][1].ele.term = DEFAULT;
	grammar[86][1].t = TERMINAL;
	grammar[86][2].ele.term = COLON;
	grammar[86][2].t = TERMINAL;
	grammar[86][3].ele.non_term = STATEMENTS;
	grammar[86][3].t = NONTERMINAL;
	grammar[86][4].ele.term = BREAK;
	grammar[86][4].t = TERMINAL;
	grammar[86][5].ele.term = SEMICOL;
	grammar[86][5].t = TERMINAL;
	
	grammar[87][0].ele.non_term = DEFAULTNT;
	grammar[87][0].t = NONTERMINAL;
	grammar[87][0].len = 1;
	grammar[87][1].ele.term = EPSILON;
	grammar[87][1].t = TERMINAL;	

	grammar[88][0].ele.non_term = ITERATIVESTMT;
	grammar[88][0].t = NONTERMINAL;
	grammar[88][0].len = 9;
	grammar[88][1].ele.term = FOR;
	grammar[88][1].t = TERMINAL;
	grammar[88][2].ele.term = BO;
	grammar[88][2].t = TERMINAL;
	grammar[88][3].ele.term = ID;
	grammar[88][3].t = TERMINAL;
	grammar[88][4].ele.term = IN;
	grammar[88][4].t = TERMINAL;
	grammar[88][5].ele.non_term = RANGE;
	grammar[88][5].t = NONTERMINAL;
	grammar[88][6].ele.term = BC;
	grammar[88][6].t = TERMINAL;
	grammar[88][7].ele.term = START;
	grammar[88][7].t = TERMINAL;
	grammar[88][8].ele.non_term = STATEMENTS;
	grammar[88][8].t = NONTERMINAL;
	grammar[88][9].ele.term = END;
	grammar[88][9].t = TERMINAL;
	
	grammar[89][0].ele.non_term = ITERATIVESTMT;
	grammar[89][0].t = NONTERMINAL;
	grammar[89][0].len = 7;
	grammar[89][1].ele.term = WHILE;
	grammar[89][1].t = TERMINAL;
	grammar[89][2].ele.term = BO;
	grammar[89][2].t = TERMINAL;
	grammar[89][3].ele.non_term = ARITHMETICORBOOLEANEXPR;
	grammar[89][3].t = NONTERMINAL;
	grammar[89][4].ele.term = BC;
	grammar[89][4].t = TERMINAL;
	grammar[89][5].ele.term = START;
	grammar[89][5].t = TERMINAL;
	grammar[89][6].ele.non_term = STATEMENTS;
	grammar[89][6].t = NONTERMINAL;
	grammar[89][7].ele.term = END;
	grammar[89][7].t = TERMINAL;

	grammar[90][0].ele.non_term = RANGE;
	grammar[90][0].t = NONTERMINAL;
	grammar[90][0].len = 3;
	grammar[90][1].ele.term = INTEGER;
	grammar[90][1].t = TERMINAL;
	grammar[90][2].ele.term = RANGEOP;
	grammar[90][2].t = TERMINAL;
	grammar[90][3].ele.term = INTEGER;
	grammar[90][3].t = TERMINAL;

	grammar[91][0].ele.non_term = OTHERMODULES;
	grammar[91][0].t = NONTERMINAL;
	grammar[91][0].len = 2;
	grammar[91][1].ele.non_term = MODULENT;
	grammar[91][1].t = NONTERMINAL;
	grammar[91][2].ele.non_term = OTHERMODULES;
	grammar[91][2].t = NONTERMINAL;

	grammar[92][0].ele.non_term = OTHERMODULES;
	grammar[92][0].t = NONTERMINAL;
	grammar[92][0].len = 1;
	grammar[92][1].ele.term = EPSILON;
	grammar[92][1].t = TERMINAL;

	grammar[93][0].ele.non_term = XXX;
	grammar[93][0].t = NONTERMINAL;
	grammar[93][0].len = 3;
	grammar[93][1].ele.term =BO;
	grammar[93][1].t = TERMINAL;
	grammar[93][2].ele.non_term = ARITHMETICORBOOLEANEXPR;
	grammar[93][2].t = NONTERMINAL;
	grammar[93][3].ele.term = BC;
	grammar[93][3].t = TERMINAL;

	// grammar[94][0].ele.non_term = VAR;
	// grammar[94][0].t = NONTERMINAL;
	// grammar[94][0].len = 2;
	// grammar[94][1].ele.term = MINUS;
	// grammar[94][1].t = TERMINAL;
	// grammar[94][2].ele.non_term = NEGVAR;
	// grammar[94][2].t = NONTERMINAL;

	grammar[94][0].ele.non_term = FACTOR;
	grammar[94][0].t = NONTERMINAL;
	grammar[94][0].len = 1;
	grammar[94][1].ele.non_term = XXX;
	grammar[94][1].t = NONTERMINAL;

	grammar[95][0].ele.non_term = NEGVAR;
	grammar[95][0].t = NONTERMINAL;
	grammar[95][0].len = 1;
	grammar[95][1].ele.term = INTEGER;
	grammar[95][1].t = TERMINAL;	

	grammar[96][0].ele.non_term = NEGVAR;
	grammar[96][0].t = NONTERMINAL;
	grammar[96][0].len = 1;
	grammar[96][1].ele.term = REAL;
	grammar[96][1].t = TERMINAL;

	grammar[97][0].ele.non_term = NEGVAR;
	grammar[97][0].t = NONTERMINAL;
	grammar[97][0].len = 2;
	grammar[97][1].ele.term = ID;
	grammar[97][1].t = TERMINAL;
	grammar[97][2].ele.non_term = WHICHID;
	grammar[97][2].t = NONTERMINAL;


	grammar[98][0].ele.non_term = VAR;
	grammar[98][0].t = NONTERMINAL;
	grammar[98][0].len = 2;
	grammar[98][1].ele.term = PLUS;
	grammar[98][1].t = TERMINAL;
	grammar[98][2].ele.non_term = NEGVAR;
	grammar[98][2].t = NONTERMINAL;

		

}


void compute_first_rec(int i,int update){
	int j,k,flag=0;
	// printf("Updating for |%d|",update);
	for(j=0;j<=GRAMMAR_LEN;j++){
		if(grammar[j][0].t==NONTERMINAL&&grammar[j][0].ele.non_term==i){
			for(k=1;k<=grammar[j][0].len;k++){
				// printf("Grammar Rule : %d\n",j);
				if(grammar[j][k].t==TERMINAL){
					first[update][grammar[j][k].ele.term]=1;
					if(grammar[j][k].ele.term==EPSILON&&grammar[j][0].len==1){
						flag=1;
					}
					break;
				}
				else{
					compute_first_rec(grammar[j][k].ele.non_term,update);
				}
				if(first[update][EPSILON]==1&&flag==0){
					first[update][EPSILON]=0;
					// if(update==N8) printf("N8 was here\n");
				}
				else{
					break;
				}
			}
		}
	}
}

void compute_first(){
	int i=0;
	for(i=0;i<noNonTerminals;i++){
		memset(first[i],0,sizeof(int)*noTerminals);
	}
	for(i=0;i<noNonTerminals;i++){
		compute_first_rec(i,i);
	}
}

void compute_follow_rec(int i,int update){
	int j,k;
	if(i==PROGRAMNT) {
		follow[update][DOLLAR]=1;
		return;
	}
	// printf("Calling with %s--%s\n",nt[i],nt[update]);
	fflush(stdout);
	for(j=0;j<=GRAMMAR_LEN;j++){
		// printf("At rule No : %d\n",j);
		fflush(stdout);	
		for(k=1;k<=grammar[j][0].len;k++){
			if(grammar[j][k].t==NONTERMINAL&&grammar[j][k].ele.non_term==i){
				k++;
				if(k>grammar[j][0].len){
						if(grammar[j][0].ele.non_term!=i)
							compute_follow_rec(grammar[j][0].ele.non_term,update);
				}
				int flag=0;
				while(k<=grammar[j][0].len){
					if(grammar[j][k].t==TERMINAL){
						follow[update][grammar[j][k].ele.term]=1;
						follow_vis[update]=1;
						flag=0;
						follow[update][EPSILON]=0;
						break;
					}
					else{
						int l;
						for(l=0;l<noTerminals;l++){
							if(first[grammar[j][k].ele.non_term][EPSILON]!=1)
							{
								follow[update][EPSILON]=0;
							}
							if(first[grammar[j][k].ele.non_term][l]==1){
								follow[update][l]=1;
								follow_vis[update]=1;
							}
						}
						if(follow[update][EPSILON]==1){
							k++;
							flag=1;
							// if(k>grammar[j][0].len) follow[update][EPSILON]=1;
							continue;
						}
						else{
							break;
						}
					}
					k++;
				}
				if(follow[update][EPSILON]==1&&flag==1){
					follow[update][EPSILON]=0;
					if(grammar[j][0].ele.non_term!=i){
						compute_follow_rec(grammar[j][0].ele.non_term,update);
					}
				}
			}
		}
	}
}

void compute_follow(){
	int i=0;
	for(i=0;i<noNonTerminals;i++){
		memset(follow[i],0,sizeof(int)*noTerminals);
	}
	follow[PROGRAMNT][DOLLAR]=1;
	follow_vis[PROGRAMNT]=1;
	for(i=1;i<noNonTerminals;i++){
		compute_follow_rec(i,i);
	}
}

void createParsingTable(){
	int i,j;
	for(i=0;i<noNonTerminals;i++){
			memset(parsingTable[i],-1,sizeof(int)*noTerminals);
	}
	for(i=0;i<noNonTerminals;i++){
		for(j=0;j<=GRAMMAR_LEN;j++){
			if(grammar[j][0].ele.non_term==i){
				if(grammar[j][0].len==1&&grammar[j][1].ele.term==EPSILON){
					int l;
					for(l=0;l<noTerminals;l++){
						if(follow[i][l]==1){
							parsingTable[i][l]=j;
						}
						// printf("Here in rule : %d\n",j);
					}
					continue;
				}
				int k,con=0;
				for(k=1;k<=grammar[j][0].len;k++){
					if(grammar[j][k].t==TERMINAL){
						parsingTable[i][grammar[j][k].ele.term]=j;
						break;
					}
					else{
						int m;
						for(m=0;m<noTerminals;m++){
							if(first[grammar[j][k].ele.non_term][m]==1&&m!=EPSILON){
								parsingTable[i][m]=j;
							}
							else if(first[grammar[j][k].ele.non_term][m]==1&&m==EPSILON){
								if(i==ANYTERM){
									printf("%s\n",nt[grammar[j][k].ele.non_term]);
								}
								con=1;
							}
						}
					}
					if(con==1){
						con=0;
						continue;
					}
					else{
						break;
					}
				}
			}
		}
	}
}

void printFollow(){
	int i , j;
	for(i=0;i<noNonTerminals;i++){
		printf("Follow for %s : ",nt[i]);
		for(j=0;j<noTerminals;j++){
			if(follow[i][j]==1) printf("%s,", ter[j]);
		}
		printf("\n");
	}
}

void printFirst(){
	int i , j;
	for(i=0;i<noNonTerminals;i++){
		printf("First for %s : ",nt[i]);
		for(j=0;j<noTerminals;j++){
			if(first[i][j]==1) printf("%s,", ter[j]);
		}
		printf("\n");
	}
}

void printParsingTable(){
	int i , j;
	printf("=============================================\n");
	for(i=0;i<noNonTerminals;i++){
		printf("For %s :\n",nt[i]);
		for(j=0;j<noTerminals;j++){
			if(parsingTable[i][j]!=-1)
				printf("%s-->%d\n",ter[j],parsingTable[i][j]);
		}
		printf("=============================================\n");
	}
}

ParseTreeNode *root;

tokenInfo *currentToken;

ParseTreeNode* getGrammarRules(ParseTreeNode *root, int grammarIndex){
    int len = grammar[grammarIndex][0].len;
    int i;
    ParseTreeNode *ptr, *temp, *prev;
    ptr = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
    ptr->tokenptr = NULL;
    ptr->nodeSymbol.t = grammar[grammarIndex][1].t;
    ptr->parent = root;
    ptr->sibling = ptr->left = NULL;
    if(ptr->nodeSymbol.t == TERMINAL){
        ptr->nodeSymbol.ele.term = grammar[grammarIndex][1].ele.term;
        ptr->isLeaf = true;
    }
    else{
        ptr->nodeSymbol.ele.non_term = grammar[grammarIndex][1].ele.non_term;
        ptr->isLeaf = false;
    }
    prev = ptr;
    for (i = 2; i <= len; i++){
        temp = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
        temp->tokenptr = NULL;
        temp->nodeSymbol.t = grammar[grammarIndex][i].t;
        temp->parent = root;
        prev->sibling = temp;
        temp->sibling = temp->left = NULL;
        if(temp->nodeSymbol.t == TERMINAL){
            temp->nodeSymbol.ele.term = grammar[grammarIndex][i].ele.term;
            temp->isLeaf = true;
        }
        else{
            temp->nodeSymbol.ele.non_term = grammar[grammarIndex][i].ele.non_term;
            temp->isLeaf = false;
        }
        prev = temp;
    }
    ParseTreeNode *xxx = ptr;

    /*    printf("\nGrammarIndex : %d ", grammarIndex);
        while(xxx != NULL){
            if(xxx->nodeSymbol.t == TERMINAL){
                printf("%s ", ter[xxx->nodeSymbol.ele.term]);
            }
            else{
                printf("%s ", nt[xxx->nodeSymbol.ele.non_term]);
            }
            xxx = xxx->sibling;
        }
    */
    return ptr;
}

HashCell *HashTable;

void populateParseTree(ParseTreeNode *root, FILE **fp, int flag){
    if (root == NULL)
        return;
    if (currentToken == NULL){
        currentToken = getToken(fp);
        if (currentToken == NULL){
            return;
        }
         //if (flag)
         //  printf("Current Token : \"%s\"\n", currentToken->lexeme.iden);
    }
    if (root->nodeSymbol.t == TERMINAL){
        // this is a leaf
        root->isLeaf = true;
        if (currentToken->token == root->nodeSymbol.ele.term){
            root->tokenptr = currentToken;
            //if (flag)
            //  printf("Token used : %s\n", ter[currentToken->token]);
            currentToken = NULL;
        }
        else if(root->nodeSymbol.ele.term != EPSILON){
        		if(currentToken->uniontype==0){
            		fprintf(stderr, "ERROR_5: The token %s for lexeme %s doesnot match at Line No: %d\n",
            						ter[currentToken->token],currentToken->lexeme.iden,currentToken->line_no);
        		}
        		else if(currentToken->uniontype==1){
        			fprintf(stderr, "ERROR_5: The token %s for lexeme %d doesnot match at Line No: %d\n",
            					ter[currentToken->token],currentToken->lexeme.num,currentToken->line_no);
        		}
        		else{
        			fprintf(stderr, "ERROR_5: The token %s for lexeme %lf doesnot match at Line No: %d\n",
            						ter[currentToken->token],currentToken->lexeme.r_num,currentToken->line_no);
        		}
        		fprintf(stderr, "Expected Token is %s\n",ter[root->nodeSymbol.ele.term]);
            fflush(stderr);
            exit(0);
        }
    }
    else{
        int grammarIndex = 
           parsingTable[root->nodeSymbol.ele.non_term][currentToken->token];
        if (grammarIndex == -1){
            fprintf(stderr, "\nERROR_6 : At Line Number : %d", 
                    currentToken->line_no);
            fprintf(stderr, " NonTerminal is %s", 
                    nt[root->nodeSymbol.ele.non_term]);
            fprintf(stderr, " and Token is %s", ter[currentToken->token]);
            fprintf(stderr, " No Rule to Expand.\n");
            exit(0);
        }
        root->left = getGrammarRules(root, grammarIndex);
        ParseTreeNode *temp;
        temp = root->left;
        while(temp != NULL){
            populateParseTree(temp, fp, flag);
            temp = temp->sibling;
        }
    }
}

ParseTreeNode* parseInputSourceCode(char *testcaseFile, int flag){
    root = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    root->isLeaf = false;
    root->tokenptr = NULL;
    root->nodeSymbol.ele.non_term = PROGRAMNT;
    root->nodeSymbol.t = NONTERMINAL;
    root->parent = root->sibling = root->left = NULL;

    FILE *fp = fopen(testcaseFile, "r");
    currentToken = NULL;
    populateParseTree(root, &fp, flag);
    if(flag)
        printf("\nSyntax checking complete,Correct Syntax!!\n");
    return root;
}

void __printParseTree(FILE *fp, ParseTreeNode *root){
    if (root == NULL)
        return;
    __printParseTree(fp, root->left);
    char na[] = "-----";
    if(root->isLeaf){
        char yes[] = "yes";
        if (root->nodeSymbol.ele.term == EPSILON){
            fprintf(fp, "%-16s %-16s %-16s %-16s %-26s %-16s %-16s\n", 
                    na, na, na, na, 
                    nt[root->parent->nodeSymbol.ele.non_term], yes,
                    ter[root->nodeSymbol.ele.term]);
        }
        else if (root->tokenptr->uniontype == 0){
            fprintf(fp, "%-16s %-16d %-16s %-16s %-26s %-16s %-16s\n",
                    root->tokenptr->lexeme.iden,
                    root->tokenptr->line_no, ter[root->tokenptr->token],
                    na, nt[root->parent->nodeSymbol.ele.non_term], yes,
                    ter[root->nodeSymbol.ele.term]);
        }
        else if (root->tokenptr->uniontype == 1){
            fprintf(fp, "%-16s %-16d %-16s %-16d %-26s %-16s %-16s\n",
                    na, root->tokenptr->line_no, 
                    ter[root->tokenptr->token], root->tokenptr->lexeme.num,
                    nt[root->parent->nodeSymbol.ele.non_term], yes,
                    ter[root->nodeSymbol.ele.term]);
        }
        else {
            fprintf(fp, "%-16s %-16d %-16s %-16lf %-26s %-16s %-16s\n", 
                    na, root->tokenptr->line_no, 
                    ter[root->tokenptr->token], root->tokenptr->lexeme.r_num,
                    nt[root->parent->nodeSymbol.ele.non_term], yes,
                    ter[root->nodeSymbol.ele.term]);
        }
    }
    else{
        char no[] = "no";
        if (root->parent == NULL){
            char roots[] = "ROOT";
            fprintf(fp, "%-16s %-16s %-16s %-16s %-26s %-16s %-16s\n",
                    na, na, na, na, 
                    roots, no,
                    nt[root->nodeSymbol.ele.non_term]);
        }
        else {
            fprintf(fp, "%-16s %-16s %-16s %-16s %-26s %-16s %-16s\n",
                    na, na, na, na, 
                    nt[root->parent->nodeSymbol.ele.non_term], no,
                    nt[root->nodeSymbol.ele.non_term]);
        }
 
    }
    ParseTreeNode *ptr = root->left;
    if(root->left == NULL)
        return;
    ptr = ptr->sibling;
    while(ptr != NULL){
        __printParseTree(fp, ptr);
        ptr = ptr->sibling;
    }
}

void printParseTree(FILE *fp){
    //FILE *fp = fopen(outfile, "w");
    __printParseTree(fp, root);
}

/*int main(int argc, char *argv[]){
    HashTable = initTable();
	populateFirstSet();
	populateFollowSet();
	populateGrammar();
	compute_first();
	compute_follow();
	createParsingTable();
    char testcase[100];
    if (argc != 3){
        printf("Usage ./a.out testcase.txt");
        return 0;
    }
    strcpy(testcase, argv[1]);
    parseInputSourceCode(testcase);
    printParseTree(argv[2]);
    printFirst();
	return 0;
}
*/
