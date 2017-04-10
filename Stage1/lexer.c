/*Team : 
 * Rishabh Joshi 2014A7PS102P
 * Deep Vyas 2014A7PS248P
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lexerDef.h"
#include "hashTable.h"


#define MAX_BUFFER_SIZE 1024
 char lexing_buff[MAX_BUFFER_SIZE+1];
 int lex_ptr=0;
 int line_no=1;
 int comm_flg=0;
 extern HashCell *HashTable;

 FILE* getStream(FILE* fp){
 	memset(lexing_buff,0,sizeof lexing_buff);
 	if(feof(fp)){
		// printf("Error Here\n");
 		return NULL;
 	}
 	if(fread(lexing_buff,sizeof(char),MAX_BUFFER_SIZE,fp)>0){
 		lexing_buff[MAX_BUFFER_SIZE]='\0';
 		int i=0;
 		return fp;
 	}
 	else{
 		fprintf(stderr,"Can't Read File\n");
 		return NULL;
 	}
 }

 tokenInfo* getToken(FILE **fp){
 	tokenInfo *token;
 	token = (tokenInfo*)malloc(sizeof(tokenInfo));
 	char lookahead;
 	int i=0;
	//ERROR CHECKING
 	while(1){
 		if(lex_ptr>=strlen(lexing_buff)||lex_ptr>=MAX_BUFFER_SIZE-1){
 			*fp = getStream(*fp);
 			if(*fp==NULL) break;
 			lex_ptr=0;
 		}
 		else{
 			while(lex_ptr<MAX_BUFFER_SIZE){
 				if(comm_flg==1){
 					int cont_star=0;
 					while(1){
 						if (lexing_buff[lex_ptr] == '*') {
 							if(lexing_buff[lex_ptr+1]=='\0'){
 								lex_ptr=-1;
 								*fp=getStream(*fp);
 								if(lexing_buff[lex_ptr+1] == '*') {
 									comm_flg = 0;
 									strcpy(token->lexeme.iden,"**");
 									token->line_no=line_no;
 									token->token=COMMENTMARK;
 									lex_ptr+=2;
 									token->uniontype=0;
									// return token;
 									break;
 								}
 							}
 							else if (lexing_buff[lex_ptr+1] == '*') {
 								comm_flg = 0;
 								strcpy(token->lexeme.iden,"**");
 								token->line_no=line_no;
 								token->token=COMMENTMARK;
 								lex_ptr+=2;
 								token->uniontype=0;
								// return token;
 								break;
 							}
 						}
 						else if(lexing_buff[lex_ptr]=='\n'){
 							line_no++;
 						}
 						lex_ptr++;
 						if(lexing_buff[lex_ptr]=='\0'){
 							lex_ptr=0;
 							*fp=getStream(*fp);
 						}
 					}
 				}

 				if(isalpha(lexing_buff[lex_ptr])){
 					char id[20]="";
 					int idptr=0;
 					while(isalpha(lexing_buff[lex_ptr])||isdigit(lexing_buff[lex_ptr])||lexing_buff[lex_ptr]=='_'){
 						id[idptr++]=lexing_buff[lex_ptr];
 						lex_ptr++;
 						if(lexing_buff[lex_ptr]=='\0'){
 							lex_ptr=0;
 							*fp=getStream(*fp);
 						}
 					}
					/*TODO : Find id in table and move forward.
					Also check keywords
					Check for length of var : Done*/
					HashNode *hash_ret = find(id,HashTable);
					if(hash_ret!=NULL){
						// printf("From Hash Table : %s\n",hash_ret->key);
						strcpy(token->lexeme.iden,hash_ret->key);
						token->line_no=line_no;
						token->token = hash_ret->token;
						token->uniontype=0;
						return token;
					}
					else{
						if(strlen(id)>8){
							fprintf(stderr,"ERROR_1: Length of identifier at line %d greater than prescribed.\n",line_no);
						}
						else
						{
							strcpy(token->lexeme.iden,id);
							token->line_no=line_no;
							token->token = ID;
							token->uniontype=0;
							return token;
						}
					}	
				}
				else if(isdigit(lexing_buff[lex_ptr])) {
					int n1=0,n2,num;
					double r_num;
					while(isdigit(lexing_buff[lex_ptr])){
						n1 = (n1*10)+(lexing_buff[lex_ptr]-'0');
						lex_ptr++;
						if(lexing_buff[lex_ptr]=='\0'){
							lex_ptr=0;
							*fp=getStream(*fp);
						}
					}
					if(lexing_buff[lex_ptr]=='.'){
						if(lexing_buff[lex_ptr+1]=='\0'){
							lex_ptr=-1;
							*fp=getStream(*fp);
						}
						if(lexing_buff[lex_ptr+1]=='.'){
							token->lexeme.num=n1;
							token->line_no=line_no;
							token->token=INTEGER;
							token->uniontype=1;
							return token;
						}
						else if(!isdigit(lexing_buff[lex_ptr+1])){
							printf("Error at Number Seqence at line:%d\n",line_no);
							return NULL;
						}
						else{
							lex_ptr++;
							int pow_dec=1;
							r_num=n1;
							while(isdigit(lexing_buff[lex_ptr])){
								r_num  = r_num + (lexing_buff[lex_ptr]-'0')*pow(10,-(pow_dec++));
								lex_ptr++;
								if(lexing_buff[lex_ptr]=='\0'){
									lex_ptr=0;
									*fp=getStream(*fp);
								}
							}
							if(lexing_buff[lex_ptr]=='e'||lexing_buff[lex_ptr]=='E'){
								int pow_ten=0;
								if(lexing_buff[lex_ptr+1]=='+'){
									lex_ptr+=2;
									while(isdigit(lexing_buff[lex_ptr])){
										pow_ten = pow_ten+ (lexing_buff[lex_ptr]-'0');
										lex_ptr++;
										if(lexing_buff[lex_ptr]=='\0'){
											lex_ptr=0;
											*fp=getStream(*fp);
										}
									}
									r_num*= pow(10,pow_ten);
								}
								else if(lexing_buff[lex_ptr+1]=='-'){
									lex_ptr+=2;
									while(isdigit(lexing_buff[lex_ptr])){
										pow_ten = pow_ten+ (lexing_buff[lex_ptr]-'0');
										lex_ptr++;
										if(lexing_buff[lex_ptr]=='\0'){
											lex_ptr=0;
											*fp=getStream(*fp);
										}
									}
									r_num*= pow(10,-pow_ten);
								}
								else if(isdigit(lexing_buff[lex_ptr+1])){
									lex_ptr+=2;
									while(isdigit(lexing_buff[lex_ptr])){
										pow_ten = pow_ten+ (lexing_buff[lex_ptr]-'0');
										lex_ptr++;
										if(lexing_buff[lex_ptr]=='\0'){
											lex_ptr=0;
											*fp=getStream(*fp);
										}
									}
									r_num*= pow(10,pow_ten);
								}
								token->lexeme.r_num=r_num;
								token->line_no=line_no;
								token->token=REAL;
								token->uniontype=2;
								return token;
							}
							else{
								token->lexeme.r_num=r_num;
								token->line_no=line_no;
								token->token=REAL;
								token->uniontype=2;
								return token;
							}
						}
					}
					else if(lexing_buff[lex_ptr]=='e'||lexing_buff[lex_ptr]=='E'){
						int pow_ten=0;
						if(lexing_buff[lex_ptr+1]=='\0'){
							lex_ptr=-1;
							*fp=getStream(*fp);
						}
						if(lexing_buff[lex_ptr+1]=='+'){
							lex_ptr+=2;
							while(isdigit(lexing_buff[lex_ptr])){
								pow_ten = pow_ten+ (lexing_buff[lex_ptr]-'0');
								lex_ptr++;
								if(lexing_buff[lex_ptr]=='\0'){
									lex_ptr=0;
									*fp=getStream(*fp);
								}
							}
							r_num*= pow(10,pow_ten);
						}
						else if(lexing_buff[lex_ptr+1]=='-'){
							lex_ptr+=2;
							while(isdigit(lexing_buff[lex_ptr])){
								pow_ten = pow_ten+ (lexing_buff[lex_ptr]-'0');
								lex_ptr++;
								if(lexing_buff[lex_ptr]=='\0'){
									lex_ptr=0;
									*fp=getStream(*fp);
								}	
							}
							r_num*= pow(10,-pow_ten);
						}
						else if(isdigit(lexing_buff[lex_ptr+1])){
							lex_ptr+=1;
							while(isdigit(lexing_buff[lex_ptr])){
								pow_ten = pow_ten+ (lexing_buff[lex_ptr]-'0');
								lex_ptr++;
								if(lexing_buff[lex_ptr]=='\0'){
									lex_ptr=0;
									*fp=getStream(*fp);
								}
							}
							r_num*= pow(10,pow_ten);
						}
						token->lexeme.r_num=r_num;
						token->line_no=line_no;
						token->token=REAL;
						token->uniontype=2;
						return token;
					}
					else{
						token->lexeme.num=n1;
						token->line_no=line_no;
						token->token=INTEGER;
						token->uniontype=1;
						return token;
					}
				}
				else if(lexing_buff[lex_ptr]=='+'){
					strcpy(token->lexeme.iden,"+");
					token->line_no=line_no;
					token->token=PLUS;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]=='-'){
					strcpy(token->lexeme.iden,"-");
					token->line_no=line_no;
					token->token=MINUS;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]=='*'){
					if(lexing_buff[lex_ptr+1]=='\0'){
						lex_ptr=-1;
						*fp=getStream(*fp);
					}
					if(lexing_buff[lex_ptr+1]=='*'){
						strcpy(token->lexeme.iden,"**");
						token->line_no=line_no;
						token->token=COMMENTMARK;
						lex_ptr+=2;

						comm_flg = 1;
						token->uniontype=0;
						// return token;
					}
					else{
						strcpy(token->lexeme.iden,"*");
						token->line_no=line_no;
						token->token=MUL;
						lex_ptr++;
						token->uniontype=0;
						return token;
					}
				}
				else if(lexing_buff[lex_ptr]=='/'){
					strcpy(token->lexeme.iden,"/");
					token->line_no=line_no;
					token->token=DIV;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]=='<'){
					if(lexing_buff[lex_ptr+1]=='\0'){
						lex_ptr=-1;
						*fp=getStream(*fp);
					}
					if(lexing_buff[lex_ptr+1]=='='){
						strcpy(token->lexeme.iden,"<=");
						token->line_no=line_no;
						token->token=LE;
						lex_ptr+=2;
						token->uniontype=0;
						return token;
					}
					else if(lexing_buff[lex_ptr+1]=='<'){
						if(lexing_buff[lex_ptr+2]=='\0'){
							lex_ptr=-2;
							*fp=getStream(*fp);
						}
						if(lexing_buff[lex_ptr+2]=='<'){
							strcpy(token->lexeme.iden,"<<<");
							token->line_no=line_no;
							token->token=DRIVERDEF;
							lex_ptr+=3;	
							token->uniontype=0;
							return token;
						}
						else{
							strcpy(token->lexeme.iden,"<<");
							token->line_no=line_no;
							token->token=DEF;
							lex_ptr+=2;	
							token->uniontype=0;
							return token;
						}
					}
					else{
						strcpy(token->lexeme.iden,"<");
						token->line_no=line_no;
						token->token=LT;
						lex_ptr++;
						token->uniontype=0;
						return token;
					}
				}
				else if(lexing_buff[lex_ptr]=='>'){
					if(lexing_buff[lex_ptr+1]=='\0'){
						lex_ptr=-1;
						*fp=getStream(*fp);
					}
					if(lexing_buff[lex_ptr+1]=='='){
						strcpy(token->lexeme.iden,">=");
						token->line_no=line_no;
						token->token=GE;
						lex_ptr+=2;
						token->uniontype=0;
						return token;
					}
					else if(lexing_buff[lex_ptr+1]=='>'){
						if(lexing_buff[lex_ptr+2]=='\0'){
							lex_ptr=-2;
							*fp=getStream(*fp);
						}
						if(lexing_buff[lex_ptr+2]=='>'){
							strcpy(token->lexeme.iden,">>>");
							token->line_no=line_no;
							token->token=DRIVERENDDEF;
							lex_ptr+=3;	
							token->uniontype=0;
							return token;
						}
						else{
							strcpy(token->lexeme.iden,">>");
							token->line_no=line_no;
							token->token=ENDDEF;
							lex_ptr+=2;	
							token->uniontype=0;
							return token;
						}
					}
					else{
						strcpy(token->lexeme.iden,">");
						token->line_no=line_no;
						token->token=GT;
						lex_ptr++;
						token->uniontype=0;
						return token;
					}
				}
				else if(lexing_buff[lex_ptr]=='='){
					if(lexing_buff[lex_ptr+1]=='\0'){
						lex_ptr=-1;
						*fp=getStream(*fp);
					}
					if(lexing_buff[lex_ptr+1]=='='){
						strcpy(token->lexeme.iden,"==");
						token->line_no=line_no;
						token->token=EQ;
						lex_ptr+=2;
						token->uniontype=0;
						return token;
					}
					else{
						fprintf(stderr,"ERROR_2: Unknown Symbol at line:%d\n",line_no);
					}
				}
				else if(lexing_buff[lex_ptr]=='!'){
					if(lexing_buff[lex_ptr+1]=='\0'){
						lex_ptr=-1;
						*fp=getStream(*fp);
					}
					if(lexing_buff[lex_ptr+1]=='='){
						strcpy(token->lexeme.iden,"!=");
						token->line_no=line_no;
						token->token=NE;
						lex_ptr+=2;
						token->uniontype=0;
						return token;
					}
					else{
						fprintf(stderr,"ERROR_2: Unknown Symbol at line:%d\n",line_no);
					}
				}
				else if(lexing_buff[lex_ptr]==':'){
					if(lexing_buff[lex_ptr+1]=='\0'){
						lex_ptr=-1;
						*fp=getStream(*fp);
					}
					if(lexing_buff[lex_ptr+1]=='='){
						strcpy(token->lexeme.iden,":=");
						token->line_no=line_no;
						token->token=ASSIGNOP;
						lex_ptr+=2;
						token->uniontype=0;
						return token;	
					}
					else{
						strcpy(token->lexeme.iden,":");
						token->line_no=line_no;
						token->token=COLON;
						lex_ptr++;
						token->uniontype=0;
						return token;
					}
				}
				else if(lexing_buff[lex_ptr]==';'){
					strcpy(token->lexeme.iden,";");
					token->line_no=line_no;
					token->token=SEMICOL;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]==','){
					strcpy(token->lexeme.iden,",");
					token->line_no=line_no;
					token->token=COMMA;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]=='['){
					strcpy(token->lexeme.iden,"[");
					token->line_no = line_no;
					token->token = SQBO;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]==']'){
					strcpy(token->lexeme.iden,"]");
					token->line_no = line_no;
					token->token = SQBC;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]=='('){
					strcpy(token->lexeme.iden,"(");
					token->line_no = line_no;
					token->token = BO;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]==')'){
					strcpy(token->lexeme.iden,")");
					token->line_no = line_no;
					token->token = BC;
					lex_ptr++;
					token->uniontype=0;
					return token;
				}
				else if(lexing_buff[lex_ptr]=='.'){
					if(lexing_buff[lex_ptr+1]=='\0'){
						lex_ptr=-1;
						*fp=getStream(*fp);
					}
					if(lexing_buff[lex_ptr+1]=='.'){
						strcpy(token->lexeme.iden,"..");
						token->line_no=line_no;
						token->token=RANGEOP;
						lex_ptr+=2;
						token->uniontype=0;
						return token;
					}
					else{
						fprintf(stderr,"ERROR_2: Unknown Symbol at Line No: %d\n",line_no);
					}
				}
				else if(lexing_buff[lex_ptr]=='\n'||lexing_buff[lex_ptr]=='\r'){
					line_no++;
					if(lexing_buff[lex_ptr]=='\r') lex_ptr++;
					lex_ptr++;
				}
				else if(lexing_buff[lex_ptr]==' '||lexing_buff[lex_ptr]=='\t'){
					while(lex_ptr<MAX_BUFFER_SIZE&&(lexing_buff[lex_ptr]==' '||lexing_buff[lex_ptr]=='\t')) 
						lex_ptr++;
				}
				else{
					// printf("%c\n",lexing_buff[lex_ptr]);
					if (lex_ptr == strlen(lexing_buff) && feof(*fp)){
						token->line_no = line_no;
						token->token = DOLLAR;
						strcpy(token->lexeme.iden, "$");
						token->uniontype = 0;
						return token;
					}
					else{
						fprintf(stderr, "ERROR_3:Unknown Pattern at Line No: %d\n",line_no);
						return NULL;
					}
				}
			}
		}
	}
}

void removeComments(char *testcaseFile){
	int i = 0, twoflag = 0, flag = 0;
	FILE *fp1 = fopen(testcaseFile, "r");
	//FILE *fp2 = fopen(cleanFile, "w");
  //  FILE *fp2 = stdout;
	char *buffer;
	buffer = (char *)malloc(1024*sizeof(char));
	char wb[1024];
	int j = 0;
	size_t size = 1024;
	while(getline(&buffer, &size, fp1) > 0){
		//
		for(i=0; i < size && buffer[i]!='\0'; i++){
			if (flag == 1) {
				if (buffer[i] == '*') {
					if(buffer[i+1] == '\0'){
						getline(&buffer, &size, fp1);
						i = 0;
						if (buffer[i] == '*') {
							flag = 0;
							i++;
						}
					}
					else if(buffer[i+1] == '*'){
						flag = 0;
						i++;
					}
				}
			}
			else{
				if (buffer[i] == '*') {
					if(buffer[i+1] == '\0'){
						getline(&buffer, &size, fp1);
						i = 0;
						if (buffer[i] == '*') {
							flag = 1;
							i+=1;
						}
					}
					else if(buffer[i+1] == '*'){
						flag = 1;
						i+=1;
					}
					else{
						//write back * MUL
						wb[j++] = buffer[i];
					}
				}
				else{
					//write back
					wb[j++] = buffer[i];
				}
				if(wb[j] == '\0'){
//					fprintf(fp2, "%s", wb);
					printf("%s", wb);
					memset(wb, 0, sizeof(wb));
					j = 0;
				}
			}

		}
	}
	fclose(fp1);
}

void printTokenList(char *testfile){
	FILE *fp = fopen(testfile, "r");
	tokenInfo *token;
	token = getToken(&fp);
	while(token != NULL){
		if (token->uniontype == 0)
			printf("%-16s %-16d\n", token->lexeme.iden, token->line_no); 
		else if (token->uniontype == 1)
			printf("%-16d %-16d\n", token->lexeme.num, token->line_no); 
		else
			printf("%-16lf %-16d\n", token->lexeme.r_num, token->line_no); 
		token = getToken(&fp);
	}
}


/*int main(){
	char fileone[]="fileone";
	char filecl[]="filecl";
	char testcase[]="testcase2.txt";
	removeComments(fileone, filecl);
	FILE *fp = fopen(testcase, "r");
	tokenInfo *newtoken;
	int i=0;
	HashTable = initTable();
	populate(HashTable);
	if(find("integer",HashTable)==NULL){
		printf("Hash Table Error\n");
	}
	for(i=0;i<200;i++){
		newtoken = getToken(&fp);
		if(newtoken==NULL) {
			printf("NULL Returned\n");
			return 0;
		}
		if(newtoken->line_no==159){
			printf("%d-->%d : %f\n", newtoken->token,newtoken->line_no,newtoken->lexeme.r_num);
			fflush(stdout);
		}
		else{
			printf("%d-->%d : %s\n", newtoken->token,newtoken->line_no,newtoken->lexeme.iden);
			fflush(stdout);
		}
	}
	// newtoken = getToken(&fp);
	// printf("hhhh");
	// if(newtoken==NULL) return 0;
	// printf("\n%d", newtoken->token);
	return 0;
}*/
