#ifndef _CODEGEN_
#define _CODEGEN_
#include "./Stage1/parserDef.h"
#include "./Stage1/lexerDef.h"
#include "ASTDef.h"
#include "HashTreeDef.h"
void codegen(ASTNode *ast_root,FILE *fp);

#endif