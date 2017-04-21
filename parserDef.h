// Batch Number - 38
// 2014A7PS102P - RISHABH JOSHI
// 2014A7PS248P - DEEP VYAS

#ifndef _PARSERDEF_
#define _PARSERDEF_

#include "lexerDef.h"

#include <stdbool.h>
#define GRAMMAR_LEN 98
#define noNonTerminals 53
#define noTerminals 57
#define maxFirstSet 20
#define maxFollowSet 20

typedef enum{
	PROGRAMNT,
	MODULEDECLARATIONS,
	OTHERMODULES,
	DRIVERMODULE,
	MODULEDECLARATION,
	MODULENT,
	MODULEDEF,
	INPUTPLIST,
	RET,
	OUTPUTPLIST,
	N1,
	N2,
	DATATYPE,
	RANGE,
	TYPE,
	STATEMENTS,
	STATEMENT,
	IOSTMT,
	VAR,
	WHICHID,
	SIMPLESTMT,
	ASSIGNMENTSTMT,
	WHICHSTMT,
	LVALUEIDSTMT,
	LVALUEARRSTMT,
	INDEX,
	MODULEREUSESTMT,
	OPTIONAL,
	IDLIST,
	N3,
	EXPRESSION,
	ARITHMETICORBOOLEANEXPR,
	ANYTERM,
	N7,
	N8,
	ARITHMETICEXPR,
	N4,
	TERM,
	N5,
	FACTOR,
	LOGICALOP,
	RELATIONALOP,
	OP1,
	OP2,
	DECLARESTMT,
	CONDITIONALSTMT,
	CASESTMTS,
	N9,
	VALUE,
	DEFAULTNT,
	ITERATIVESTMT,
	NEGVAR,
	XXX
}NonTerminal;

typedef TokenType Terminal;

typedef union{
	Terminal term;
	NonTerminal non_term;
}GNodeType;

typedef enum{
	TERMINAL,NONTERMINAL
}tag;

struct GrammarNode{
	GNodeType ele;
	tag t;
	// struct GrammarNode *next;
	int len;
};
typedef struct GrammarNode GrammarNode;

typedef struct {
	int grammarindex;
	Terminal t;
}FirstAndFollowNode;

typedef struct{
	FirstAndFollowNode first[noNonTerminals][maxFirstSet];
	FirstAndFollowNode follow[noNonTerminals][maxFollowSet];
}FirstAndFollow;

struct ParseTreeNode{
    tokenInfo *tokenptr;
    bool isLeaf;
    GrammarNode nodeSymbol;

    // FOR AST SIGN PROPAGATION
    int parseTreeSign;

    struct ParseTreeNode *parent, *sibling, *left;
};
typedef struct ParseTreeNode ParseTreeNode;

void populateFirstSet();
void populateFollowSet();
void populateGrammar();
void compute_first_rec(int i,int update);
void compute_first();
void compute_follow_rec(int i,int update);
void compute_follow();
void createParsingTable();
void printFollow();
void printFirst();
void printParsingTable();
ParseTreeNode* getGrammarRules(ParseTreeNode *root, int grammarIndex);
void populateParseTree(ParseTreeNode *root, FILE **fp, int flag);
ParseTreeNode* parseInputSourceCode(char *testcaseFile, int flag);
void __printParseTree(FILE *fp, ParseTreeNode *root);
void printParseTree(FILE *fp);


#endif
