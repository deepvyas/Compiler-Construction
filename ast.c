#include <stdio.h>
#include <stdlib.h>
#include "ASTDef.h"

ASTNode* create_ast_node(){
	ASTNode* node;
	node = (ASTNode*)malloc(1*sizeof(ASTNode));
	node->child=NULL;
	node->parent=NULL;
	node->sibling=NULL;
	node->tokenptr=NULL;
	return node;
}

int extract_type(ParseTreeNode *node){
	int type= node->left->nodeSymbol.ele.term;
	if(type==INTEGER) return 0;
	else if(type==REAL) return 1;
	else if(type==BOOLEAN) return 2;
	else if(type==ARRAY) return 3;
}

ASTNode* make_id_node(ParseTreeNode *idnode,ParseTreeNode *dnode){
	ASTNode *head;
	head= create_ast_node();
	head->t=TERMINAL;
	head->gnode.term=idnode->nodeSymbol.ele.term;
	head->tokenptr=idnode->tokenptr;
	head->dtype = extract_type(dnode);
	if(head->dtype==3){
		ParseTreeNode *rangenode=dnode->left->sibling->sibling;
		ParseTreeNode *it = dnode->left;
		while(it->sibling!=NULL){
			it=it->sibling;
		}
		head->arrtype= extract_type(it);
		head->lrange= rangenode->left->tokenptr->lexeme.num;
		head->rrange= rangenode->left->sibling->sibling->tokenptr->lexeme.num;
	}
	return head;
}

ASTNode* compressList(ParseTreeNode *proot,ASTNode *parent){
	ASTNode *head;
	ParseTreeNode *itr=proot;
	int nt= proot->nodeSymbol.ele.non_term;
	if(nt==INPUTPLIST||nt==OUTPUTPLIST){
		ParseTreeNode *typenode = itr->left->sibling->sibling;
		head = make_id_node(itr->left,typenode);
		head->startscope=parent->startscope;
		head->endscope=parent->endscope;
		head->parent=parent;
		int cnt=1;
		ParseTreeNode *n1=typenode->sibling;
		ASTNode *ast_it=head;
		while(n1->left->nodeSymbol.ele.term!=EPSILON){
			ParseTreeNode *temp=n1->left->sibling;
			ast_it->sibling=make_id_node(temp,temp->sibling->sibling);
			ast_it->sibling->startscope=parent->startscope;
			ast_it->sibling->endscope=parent->endscope;
			ast_it->sibling->parent=parent;
			cnt++;
			ast_it=ast_it->sibling;
			n1=temp->sibling->sibling->sibling;
		}
		parent->listcount=cnt;
		return head;
	}
	else if(nt==IDLIST&&parent->gnode.non_term==DECLARESTMT){
		ParseTreeNode *typenode=itr->sibling->sibling;
		head = make_id_node(itr->left,typenode);
		head->startscope=parent->startscope;
		head->endscope=parent->endscope;
		head->parent=parent;
		int cnt=1;
		ParseTreeNode *n1=itr->left->sibling;
		ASTNode *ast_it=head;
		while(n1->left->nodeSymbol.ele.term!=EPSILON){
			ParseTreeNode *temp=n1->left->sibling;
			ast_it->sibling=make_id_node(temp,typenode);
			ast_it->sibling->startscope=parent->startscope;
			ast_it->sibling->endscope=parent->endscope;
			ast_it->sibling->parent=parent;
			cnt++;
			ast_it=ast_it->sibling;
			n1=temp->sibling;
		}
		parent->listcount=cnt;
		return head;
	}
	else if(nt==IDLIST&&parent->gnode.non_term==IDLIST){
		head = create_ast_node();
		int cnt=1;
		head->parent=parent;
		head->t=itr->left->nodeSymbol.t;
		head->gnode.term=itr->left->nodeSymbol.ele.term;
		head->tokenptr=itr->left->tokenptr;
		head->startscope=parent->startscope;
		head->endscope=parent->endscope;
		ParseTreeNode *it=itr->left->sibling;
		ASTNode *ast_it=head;
		while(it->left->nodeSymbol.ele.non_term!=EPSILON){
			ASTNode *temp=create_ast_node();
			cnt++;
			temp->parent=parent;
			temp->t=it->left->sibling->nodeSymbol.t;
			temp->gnode.term=it->left->sibling->nodeSymbol.ele.term;
			temp->tokenptr=it->left->sibling->tokenptr;
			temp->startscope=parent->startscope;
			temp->endscope=parent->endscope;
			ast_it->sibling=temp;
			ast_it=ast_it->sibling;
			it=it->left->sibling->sibling;
		}
		parent->listcount=cnt;
		return head;
	}
	else if(nt==CASESTMTS){
		head= create_ast_node();
		head->parent=parent;
		head->gnode.term=CASE;
		head->t=TERMINAL;
		head->tokenptr=itr->left->sibling->tokenptr;
		if(itr->left->sibling->nodeSymbol.ele.term==INTEGER){
			head->value.num= itr->left->sibling->tokenptr->lexeme.num;
		}
		else if(itr->left->sibling->nodeSymbol.ele.term==TRUE){
			head->value.tval=1;
		}
		else{
			head->value.tval=0;
		}
		ParseTreeNode *stmts=itr->left->sibling->sibling->sibling;
		head->child=genAST(stmts,head);
		int cnt=1;
		ParseTreeNode *n1=stmts->sibling->sibling->sibling;
		ASTNode *ast_it=head;
		while(n1->left->nodeSymbol.ele.term!=EPSILON){
			ASTNode *temp;
			temp= create_ast_node();
			temp->parent=parent;
			temp->gnode.term=CASE;
			temp->t=TERMINAL;
			temp->tokenptr=n1->left->sibling->tokenptr;
			if(n1->left->sibling->nodeSymbol.ele.term==INTEGER){
				temp->value.num= n1->left->sibling->tokenptr->lexeme.num;
			}
			else if(n1->left->sibling->nodeSymbol.ele.term==TRUE){
				temp->value.tval=1;
			}
			else{
				temp->value.tval=0;
			}
			ParseTreeNode *stmts1=itr->left->sibling->sibling->sibling;
			temp->child=genAST(stmts1,temp);
			cnt++;
			ast_it->sibling=temp;
			ast_it=ast_it->sibling;
			n1=stmts1->sibling->sibling->sibling;
		}
		parent->listcount=cnt;
		return head;
	}
	else{
		if(proot->left->nodeSymbol.t==TERMINAL&&proot->left->nodeSymbol.ele.term==EPSILON){
			head = create_ast_node();
			head->t=TERMINAL;
			head->gnode.term=EPSILON;
			head->parent=parent;
			head->child=NULL;
			head->sibling=NULL;
			head->startscope=parent->startscope;
			head->endscope=parent->endscope;
			return head;
		}
		if(itr->nodeSymbol.t==NONTERMINAL){
			head = genAST(proot->left,parent);
			head->parent=parent;
			head->startscope=parent->startscope;
			head->endscope=parent->endscope;
		}
		int cnt=1;
		itr=itr->left->sibling;
		ASTNode *ast_it=head;
		while(itr->left->nodeSymbol.ele.term!=EPSILON){
			ast_it->startscope=parent->startscope;
			ast_it->endscope=parent->endscope;
			ast_it->sibling = genAST(itr->left,parent);
			cnt++;
			ast_it=ast_it->sibling;
			itr=itr->left->sibling;
		}
		parent->listcount=cnt;
		return head;
	}
}

ASTNode* resolve_var(ParseTreeNode *node,ASTNode *parent){
	ASTNode *temp=create_ast_node();
	temp->parent=parent;
	temp->t=node->nodeSymbol.t;
	temp->gnode.non_term=node->nodeSymbol.ele.non_term;
	ParseTreeNode *child=node->left;
	temp->sign=1;
	temp->startscope=parent->startscope;
	temp->endscope=parent->endscope;
	if(child->nodeSymbol.t==TERMINAL&&
		(child->nodeSymbol.ele.term==MINUS||child->nodeSymbol.ele.term==PLUS)){
		temp= resolve_var(child->sibling,parent);
	if(child->nodeSymbol.ele.term==MINUS) temp->sign=-1;
	}
	else{
		if(child->nodeSymbol.t==TERMINAL&&(child->nodeSymbol.ele.term==INTEGER||child->nodeSymbol.ele.term==REAL)){
			temp->tokenptr=child->tokenptr;
			temp->vartype=0;
			if(child->nodeSymbol.ele.term==INTEGER)
				temp->value.num= child->tokenptr->lexeme.num;
			else
				temp->value.r_num= child->tokenptr->lexeme.r_num;
		}
		else{
			ParseTreeNode *which = child->sibling->left;
			if(which->nodeSymbol.t==TERMINAL&&which->nodeSymbol.ele.term==EPSILON){
				temp->tokenptr=child->tokenptr;
				temp->vartype=1;
			}
			else{
				temp->vartype=2;
				temp->child=create_ast_node();
				temp->child->parent=temp;
				temp->child->gnode.term=child->nodeSymbol.ele.term;
				temp->child->t=child->nodeSymbol.t;
				temp->child->tokenptr=child->tokenptr;
				ASTNode *sib=create_ast_node();
				sib->parent=temp;
				sib->gnode.term=which->sibling->nodeSymbol.ele.term;
				sib->t=which->sibling->nodeSymbol.t;
				sib->tokenptr=which->sibling->tokenptr;
				temp->child->sibling=sib;
			}
		}
	}
	return temp;
}

ASTNode* resolve_assgn_stmt(ParseTreeNode *node,ASTNode *parent){
	ASTNode *head=create_ast_node();
	head->parent=parent;
	head->t=node->nodeSymbol.t;
	head->gnode.non_term=node->nodeSymbol.ele.non_term;
	ParseTreeNode *whichstmt=node->left->sibling;
	head->startscope=parent->startscope;
	head->endscope=parent->endscope;
	ASTNode *var=create_ast_node();
	var->parent=head;
	var->t=NONTERMINAL;
	var->gnode.non_term=VAR;
	var->startscope=parent->startscope;
	var->endscope=parent->endscope;
	if(whichstmt->left->nodeSymbol.ele.non_term==LVALUEIDSTMT){
		var->vartype=1;
		var->tokenptr=node->left->tokenptr;
		head->child=var;
		head->child->sibling= genAST(whichstmt->left->left->sibling,head);
	}
	else{
		var->vartype=2;
		var->child=create_ast_node();
		var->child->parent=var;
		var->child->gnode.term=node->left->nodeSymbol.ele.term;
		var->child->t=node->left->nodeSymbol.t;
		var->child->tokenptr=node->left->tokenptr;
		var->child->startscope=var->startscope;
		var->child->endscope=var->endscope;
		ParseTreeNode *lvaluearrchild = whichstmt->left->left;
		ASTNode *sib=create_ast_node();
		sib->parent=var;
		sib->gnode.term=lvaluearrchild->sibling->left->nodeSymbol.ele.term;
		sib->t=lvaluearrchild->sibling->left->nodeSymbol.t;
		sib->tokenptr=lvaluearrchild->sibling->left->tokenptr;
		var->child->sibling=sib;
		head->child=var;
		head->child->sibling= genAST(lvaluearrchild->sibling->sibling->sibling->sibling,head);
	}
	return head;
}

ASTNode* resolve_module_stmt(ParseTreeNode *node,ASTNode *parent){
	ASTNode *head=create_ast_node();
	head->parent=parent;
	head->t=node->nodeSymbol.t;
	head->gnode.non_term=node->nodeSymbol.ele.non_term;
	head->startscope=parent->startscope;
	head->endscope=parent->endscope;
	ASTNode *idlst=create_ast_node();
	idlst->parent=head;
	idlst->t=NONTERMINAL;
	idlst->gnode.non_term=IDLIST;
	idlst->startscope=parent->startscope;
	idlst->endscope=parent->endscope;
	ASTNode *paramlst=create_ast_node();
	paramlst->parent=head;
	paramlst->t=NONTERMINAL;
	paramlst->gnode.non_term=IDLIST;
	paramlst->startscope=parent->startscope;
	paramlst->endscope=parent->endscope;
	// CHECK FOR OPTIONAL
	head->child=idlst;
	ParseTreeNode *it=node->left;
	while(it->nodeSymbol.ele.non_term!=IDLIST){
		it=it->sibling;
	}
	paramlst->child=compressList(it,paramlst);
	head->child->sibling= paramlst;
	if(node->left->left->nodeSymbol.ele.non_term==EPSILON){
		idlst->child=NULL;
		idlst->listcount=0;
	}
	else{
		idlst->child = compressList(node->left->left->sibling,idlst);
	}
	return head;
}

ASTNode* resolve_exp_recursive(ParseTreeNode *node,ASTNode *parent){
	ASTNode *head = create_ast_node();
	head->parent=parent;
	head->startscope=parent->startscope;
	head->endscope=parent->endscope;
	ParseTreeNode *pparent=node->parent;
	if(node->nodeSymbol.ele.non_term==pparent->nodeSymbol.ele.non_term){
		ParseTreeNode *op=pparent->left->left;
		head->t=op->nodeSymbol.t;
		head->gnode.term=op->nodeSymbol.ele.term;
		head->tokenptr=op->tokenptr;
		head->rop= genAST(pparent->left->sibling,head);
		head->lop= resolve_exp_recursive(pparent,head);
		head->child=head->lop;
		head->child->sibling=head->rop;
		return head;
	}
	else{
		return genAST(pparent->left,parent);
	}
}

ASTNode* genAST(ParseTreeNode *proot,ASTNode *parent){
	if(proot==NULL) return NULL;
	ASTNode *root = create_ast_node();
	root->t=proot->nodeSymbol.t;
	root->gnode.non_term=proot->nodeSymbol.ele.non_term;
	root->parent=parent;
	root->tokenptr=NULL;
	/*Program Non-Terminal which stores all child*/
	if(proot->nodeSymbol.t==NONTERMINAL && proot->nodeSymbol.ele.non_term==PROGRAMNT){
		root->child=genAST(proot->left,root);
		ASTNode *ast_it=root->child;
		ParseTreeNode *it=proot->left;
		while(it!=NULL){
			ast_it->sibling = genAST(it->sibling,root);
			ast_it=ast_it->sibling;
			it=it->sibling;
		}
		return root;
	}
	/* ** ModuleDeclarations and OtherModules List of Module Nodes as child*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&
		(proot->nodeSymbol.ele.non_term==MODULEDECLARATIONS||
			proot->nodeSymbol.ele.non_term==OTHERMODULES)){
		root->child = compressList(proot,root);
		return root;
  }
  /*tokenptr which has ID*/
  else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==MODULEDECLARATION){
  	root->child=NULL;
  	root->tokenptr = proot->left->sibling->sibling->tokenptr;
		// Enter into Symbol Table;
  	return root;
  }
  /*drivermodule node has child MODULEDEF and tokenptr for driver.*/
  else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==DRIVERMODULE){
  	root->tokenptr = proot->left->sibling->tokenptr;
		root->scope=1; //TODO : SCOPE NUMBER FIXING
		ParseTreeNode *it=proot->left;
		while(it->sibling!=NULL){
			it=it->sibling;
		}
		root->child = genAST(it,root);
		return root;
	}
	/*Has name of module in tokenptr and three children INPUTPLIST OUTPUTLIST and MODULEDEF*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==MODULENT){
		ParseTreeNode *it = proot->left;
		root->tokenptr= it->sibling->sibling->tokenptr;
		root->scope = 1; //TODO : SCOPE NUMBER FIXING
		while(it->nodeSymbol.ele.non_term!=INPUTPLIST) it=it->sibling;
		root->child=	genAST(it,root);
		//RET
		it=it->sibling->sibling->sibling;
		if(it->left->nodeSymbol.ele.term==EPSILON){
			root->child->sibling = create_ast_node();
			root->child->sibling->gnode.non_term=it->nodeSymbol.ele.non_term;
			root->child->sibling->t=it->nodeSymbol.t;
			root->child->sibling->child= create_ast_node();
			root->child->sibling->child->t= TERMINAL;
			root->child->sibling->child->gnode.term= EPSILON;
		}
		else{
			root->child->sibling = genAST(it->left->sibling->sibling,root);
		}
		root->child->sibling->sibling = genAST(it->sibling,root);
		return root;
	}
	/*Has a list of child as IDs*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&
		(proot->nodeSymbol.ele.non_term==INPUTPLIST||proot->nodeSymbol.ele.non_term==OUTPUTPLIST)){
		root->child = compressList(proot,root);
		return root;
	}
	/*Has a list of child each of which is a STATEMENT*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==MODULEDEF){
		root->startscope = proot->left->tokenptr->line_no;
		root->endscope= proot->left->sibling->sibling->tokenptr->line_no;
		ASTNode *temp=genAST(proot->left->sibling,root);
		root->child = temp->child;
		ASTNode *it=root->child;
		while(it!=NULL){
			it->parent=root;
			it->startscope=root->startscope;
			it->endscope=root->endscope;
			it=it->sibling;
		}
		root->scope= 1; // ADD SCOPE;
		return root;
	}
	/*Has a dummy node which has child as list of STATEMENT*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==STATEMENTS){
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		root->child = compressList(proot,root);
		root->scope=1; //ADD SCOPE
	}
	/* A node which has stmttype as type of statement and corresponding child structure from the stmttype*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==STATEMENT){
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		ASTNode *temp = genAST(proot->left,root);
		root->child=temp->child;
		root->stmttype = temp->stmttype;
		ASTNode *it=root->child;
		while(it!=NULL){
			it->parent=root;
			it=it->sibling;
		}
		return root;
	}
	/* Has a child for type of IO and sibling which :
			1. has a node with tokenptr for ID if GET_VALUE
			2. has a var node if PRINT */
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==IOSTMT){
		root->stmttype=IOSTMT;
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		root->child= create_ast_node();
		root->child->parent=root;
		root->child->t=TERMINAL;
		//root->child->tokenptr=proot->tokenptr;
		root->child->gnode.term=proot->left->nodeSymbol.ele.term;
		if(root->child->gnode.term==GET_VALUE){
			ASTNode *temp= create_ast_node();
			temp->t=TERMINAL;
			temp->parent=root;
			temp->gnode.term=proot->left->sibling->sibling->nodeSymbol.ele.term;
			temp->tokenptr=proot->left->sibling->sibling->tokenptr;
			temp->startscope=root->startscope;
			temp->endscope=root->endscope;
			root->child->sibling=temp;
		}
		else{
			root->child->sibling = resolve_var(proot->left->sibling->sibling,root);
		}
		return root;
	}
	/*Has a child for ASSIGNMENTSTMT or MODULEREUSESTMT*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==SIMPLESTMT){
		root->stmttype=SIMPLESTMT;
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		root->child = genAST(proot->left,root);
		return root;
	}
	/*Has child with one node for VAR and other for EXPRESSION*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==ASSIGNMENTSTMT){
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		ASTNode *temp = resolve_assgn_stmt(proot,root);
		root->child=temp->child;
		ASTNode *it=root->child;
		while(it!=NULL){
			it->parent=root;
			it=it->sibling;
		}
		return root;
	}
	/*Node has tokenptr for ID of MODULE and two child one is IDLIST(list_of_assgn) and other IDLIST(list_of_id)*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==MODULEREUSESTMT){
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		ASTNode *temp= resolve_module_stmt(proot,root);
		root->t=temp->t;
		root->gnode.non_term=temp->gnode.non_term;
		root->tokenptr= proot->left->sibling->sibling->sibling->tokenptr;
		root->child = temp->child;
		ASTNode *it= root->child;
		while(it!=NULL){
			it->parent=root;
			it=it->sibling;
		}
		return root;
	}
	/*List of IDs with dtype field */
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==DECLARESTMT){
		root->stmttype=DECLARESTMT;
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		ParseTreeNode *idlist=proot->left->sibling;
		root->child = compressList(idlist,root);
		return root;
	}
	/*Has one child as ID on which case is based, next child of CASESTMTS and next if present of DEFAULTNT*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==CONDITIONALSTMT){
		root->stmttype=CONDITIONALSTMT;
		ParseTreeNode *id=proot->left->sibling->sibling;
		ParseTreeNode *casestmt=id->sibling->sibling->sibling;
		root->child = create_ast_node();
		root->child->parent=root;
		root->child->t=id->nodeSymbol.t;
		root->child->gnode.term=id->nodeSymbol.ele.term;
		root->child->tokenptr=id->tokenptr;
		root->child->sibling = genAST(casestmt,root);
		root->child->startscope=parent->startscope;
		root->child->endscope=parent->endscope;
		root->startscope= id->sibling->sibling->tokenptr->line_no;
		root->endscope= casestmt->sibling->sibling->tokenptr->line_no;
		root->child->sibling->sibling = genAST(casestmt->sibling,root);
		return root;
	}
	/* Two children if for then id and stmnts, if while then expressions and stmnts */
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==ITERATIVESTMT){
		root->stmttype=ITERATIVESTMT;
		ParseTreeNode *child=proot->left;
		if(child->nodeSymbol.ele.term==FOR){
			root->looptype=FOR;
			ParseTreeNode *range= child->sibling->sibling->sibling->sibling;
			root->child=create_ast_node();
			root->child->tokenptr = child->sibling->sibling->tokenptr;
			root->child->lrange = range->left->tokenptr->lexeme.num;
			root->child->rrange= range->left->sibling->sibling->tokenptr->lexeme.num;
			root->child->startscope=parent->startscope;
			root->child->endscope=parent->endscope;
			root->startscope=range->sibling->sibling->tokenptr->line_no;
			root->endscope=range->sibling->sibling->sibling->sibling->tokenptr->line_no;
			root->child->sibling = genAST(range->sibling->sibling->sibling,root);
		}
		else{
			root->looptype=WHILE;
			ParseTreeNode *aexp=proot->left->sibling->sibling;
			root->startscope=parent->startscope;
			root->endscope=parent->endscope;
			root->child = genAST(aexp,root);
			root->startscope=aexp->sibling->sibling->tokenptr->line_no;
			root->endscope=aexp->sibling->sibling->sibling->sibling->tokenptr->line_no;
			root->child->sibling = genAST(aexp->sibling->sibling->sibling,root);
		}
		return root;
	}
	/*Has a list of CASE nodes which have :
			1. tokenptr and value for case
			2. STATEMENTS child which has a list of STATEMENT child which have corresponding statement as child.*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==CASESTMTS){
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		ASTNode *temp= compressList(proot,root);
		ASTNode *it;
		root->child = temp->child;
		it=root->child;
		while(it!=NULL){
			it->parent=root;
			it=it->sibling;
		}
		return root;
	}
	/*Has child STATEMENTS which has child STATEMENT which has corresponding behaviour*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==DEFAULTNT){
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		if(proot->nodeSymbol.ele.non_term==EPSILON){
			root->child=NULL;
		}
		else{
			ASTNode *temp= genAST(proot->left->sibling->sibling,root);
			ASTNode *it;
			root->child = temp->child;
			it=root->child;
			while(it!=NULL){
				it->parent=root;
				it=it->sibling;
			}
		}
		return root;
	}
	/*Has child as corresponding tree of expression*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==EXPRESSION){
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		ASTNode *aoeb = genAST(proot->left,root);
		root->child=aoeb;
		ASTNode *it=root->child;
		while(it!=NULL){
			it->parent=root;
			it=it->sibling;
		}
		return root;
	}
	/**/
	else if(proot->nodeSymbol.t==NONTERMINAL&&(
		proot->nodeSymbol.ele.non_term==ARITHMETICORBOOLEANEXPR||
		proot->nodeSymbol.ele.non_term==ANYTERM||
		proot->nodeSymbol.ele.non_term==ARITHMETICEXPR||
		proot->nodeSymbol.ele.non_term==TERM)){
		//Go till EPSILON
		ParseTreeNode *tempPtr = proot->left->sibling;
		while(tempPtr->left->nodeSymbol.ele.non_term!=EPSILON){
			tempPtr=tempPtr->left->sibling->sibling;
		}
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		ASTNode *tempast = resolve_exp_recursive(tempPtr,root);   
    // do herea
    return tempast;
	}
	/*Has sign of node and corresponding VAR node.*/
	else if(proot->nodeSymbol.t==NONTERMINAL&&proot->nodeSymbol.ele.non_term==FACTOR){
		ParseTreeNode *xxx;
		if(proot->left->nodeSymbol.ele.term==MINUS){
			root->sign=-1;
			xxx=proot->left->sibling;
		}
		else{
			xxx=proot->left;
		}
		if(xxx->left->nodeSymbol.t==TERMINAL&&xxx->left->nodeSymbol.ele.term==BO){
			return genAST(xxx->left->sibling,parent);
		}
		else{
			return resolve_var(xxx->left,parent);
		}
	}
	/**/
	else{
		ASTNode *root = create_ast_node();
		root->t=proot->nodeSymbol.t;
		root->gnode.non_term=proot->nodeSymbol.ele.non_term;
		root->parent=parent;
		root->startscope=parent->startscope;
		root->endscope=parent->endscope;
		return root;
	}
}

void _printAST(ASTNode *ast_root){
	if(ast_root==NULL) return;
	if(ast_root->child!=NULL){
		printf("Child of %d : %d",ast_root->gnode.non_term,ast_root->child->gnode.non_term);
	}
	else if(ast_root->lop!=NULL&&ast_root->rop!=NULL){
		printf("Left Op is : %d and Right Op is : %d \n",ast_root->lop->gnode.non_term,ast_root->rop->gnode.non_term);
		_printAST(ast_root->lop);
		_printAST(ast_root->rop);
		return;
	}
	else{
		if(ast_root->tokenptr!=NULL){
			if(ast_root->gnode.term==ID){
				printf("Token at %d : %s and datatype is %d and ||scope|| is : %d--%d\n",
					ast_root->gnode.non_term,ast_root->tokenptr->lexeme.iden,
					ast_root->dtype,ast_root->startscope,ast_root->endscope);
			}
			else{
				printf("Token at %d : %s and ||scope|| is : %d--%d\n",
					ast_root->gnode.non_term,ast_root->tokenptr->lexeme.iden,
					ast_root->startscope,ast_root->endscope);
			}
		}
		else{
			printf("Child of %d :\n",ast_root->gnode.non_term);
		}
		return;
	}
	ASTNode *it=ast_root->child->sibling;
	while(it!=NULL){
		printf(", %d",it->gnode.non_term);
		it=it->sibling;
	}
	printf("\n");
	it=ast_root->child;
	while(it!=NULL){
		_printAST(it);
		it=it->sibling;
	}
}

int main(int argc,char* argv[]){
	initTable();
	populateGrammar();
	compute_first();
	compute_follow();
	createParsingTable();
	ParseTreeNode *proot=parseInputSourceCode(argv[1],0);
	ASTNode *ast_root=genAST(proot,NULL);
	_printAST(ast_root);
	return 0;
}
