#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../common/common.h"
#include "pl0_lex.h"

const char * TOKEN_RESERVED_WORDS[NRW] = {"var", "const", "procedure", "begin", "end", "if", "then", "do", "while", "call", "odd"};
const char * TOKEN_SYMBOLS[NSYM] = { "+", "-", "*", "/", "=", "!=", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":=" };

PL0Lex * PL0Lex_create(PL0Compiler * parent)
{
	PL0Lex * lex = (PL0Lex *) calloc(1, sizeof(PL0Lex));
	lex->compiler = parent;
	lex->last_token_type = TOKEN_NULL;
	lex->last_num = 0;
	lex->position = 0;
	lex->newchar = '0';
	lex->line = 1;

	return lex;
}

BOOL PL0Lex_destroy(PL0Lex * lex)
{
	free(lex);
	return TRUE;
}

BOOL PL0Lex_get_token(PL0Lex * lex)
{
	lex->newchar = fgetc(example);
	if(lex->newchar != -1)
		return TRUE;
	else
		return FALSE;
}

BOOL IsTokenASymbol(PL0Lex *lex){
	BOOL check = FALSE;
	for(int i = 0; i < NSYM; i++){
		if(strcmp(lex->last_id,TOKEN_SYMBOLS[i])==0)
			check = TRUE;
	}
	if(check == TRUE)
		return TRUE;
	else 
		return FALSE;
}

BOOL IsTokenAReservedWord(PL0Lex *lex){
	BOOL check = FALSE;
	for(int i=0; i<NRW; i++){
		if(strcmp(lex->last_id,TOKEN_RESERVED_WORDS[i])==0)
			check = TRUE;
	}
	if(check == TRUE)
		return TRUE;
	else
		return FALSE;
}

BOOL IsVacant(char ch){
	if(ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
		return TRUE;
	else
		return FALSE;
}

BOOL IsSym(char ch){
	if(ch=='+' || ch=='-' || ch=='*' || ch=='/' || ch==';' || ch==':' || ch=='(' || ch==')' || ch=='.' || ch=='!' || ch=='=')
		return TRUE;
	else 
		return FALSE;
}

void printLex(PL0Lex *lex){
	switch(lex->last_token_type){
		case(TOKEN_SYMBOL):{
			if(IsTokenASymbol(lex)){
				printf("Symbol:");
				for(int i=0; i<11; i++)
					printf(" ");
				printf("%s",lex->last_id);
				for(int i=0; i<MAX_ID_LEN-strlen(lex->last_id);i++)
					printf(" ");
				printf("%d: %d - %d",lex->line,lex->row-strlen(lex->last_id)+1, lex->row);
			}
			else{
				printf("error\n");
			}
			break;
		}

		case(TOKEN_IDENTIFIER):{
			if(IsTokenAReservedWord(lex)){
				printf("Reserved Word:");
				for(int i=0; i<4; i++)
					printf(" ");
			}
			else{
				printf("Identifier:");
				for(int i=0; i<7; i++)
					printf(" ");
			}
			printf("%s", lex->last_id);
			for(int i=0; i<MAX_ID_LEN-strlen(lex->last_id); i++)
				printf(" ");
			printf("%d: %d - %d", lex->line, lex->row-strlen(lex->last_id)+1,lex->row);
			break;
		}

		case(TOKEN_NUMBER):{
			printf("Number:");
			for(int i=0; i<11; i++)
				printf(" ");
			printf("%s",lex->last_id);
			for(int i=0; i<MAX_ID_LEN-strlen(lex->last_id); i++)
				printf(" ");
			printf("%d: %d - %d", lex->line, lex->row-strlen(lex->last_id)+1, lex->row);
			break;
		}
	}
	lex->last_token_type = TOKEN_NULL;
	lex->position = 0;
	for(int i=0; i<MAX_ID_LEN; i++)
		lex->last_id[i] = '\0';
	printf("\n");
}

void LEX(PL0Lex *lex){
	switch(lex->last_token_type){
		case(TOKEN_NULL):{										//NULL
			if(isalpha(lex->newchar) || lex->newchar == '_'){	//letter or _
				lex->last_token_type = TOKEN_IDENTIFIER;
				lex->last_id[lex->position++] = lex->newchar;
				lex->row;
			}
			else if(isdigit(lex->newchar)){						//digit
				lex->last_token_type = TOKEN_NUMBER;
				lex->last_id[lex->position++] = lex->newchar;
				lex->row++;
			}
			else if(IsVacant(lex->newchar)){					//vacant
				if(lex->newchar == '\n'){
					lex->line++;
					lex->row=0;
				}
				else
					lex->row++;
			}
			else if(IsSym(lex->newchar)){						//symbol
				lex->last_token_type = TOKEN_SYMBOL;
				lex->last_id[lex->position++] = lex->newchar;
				lex->row++;
			}
		}
		break;

		case(TOKEN_IDENTIFIER):{								//identifier
			if(isalpha(lex->newchar) || lex->newchar == '_' || isdigit(lex->newchar)){		//letter or digit or _
				lex->last_id[lex->position++] = lex->newchar;
				lex->row++;
			}
			else if(IsVacant(lex->newchar)){					//vacant
				printLex(lex);
				if(lex->newchar == '\n'){
					lex->line++;
					lex->row=0;
				}
				else
					lex->row++;
			}
			else if(IsSym(lex->newchar)){						//symbol
				printLex(lex);
				lex->last_token_type = TOKEN_SYMBOL;
				lex->last_id[lex->position++] = lex->newchar;
				lex->row++;
			}
		}
		break;

		case(TOKEN_NUMBER):{									//digit
			if(isdigit(lex->newchar)){							//digit
				lex->last_id[lex->position++] = lex->newchar;
				lex->row++;
			}
			else if(IsVacant(lex->newchar)){					//vacant
				printLex(lex);
				if(lex->newchar == '\n'){
					lex->line++;
					lex->row = 0;
				}
				else
					lex->row++;
			}
			else if(IsSym(lex->newchar)){						//symbol
				printLex(lex);
				lex->last_token_type = TOKEN_SYMBOL;
				lex->last_id[lex->position] = lex->newchar;
				lex->row++;
			}
			else if(isalpha(lex->newchar) || lex->newchar == '_'){//letter or _
				printLex(lex);
				lex->last_token_type = TOKEN_SYMBOL;
				lex->last_id[lex->position++] = lex->newchar;
				lex->row++;
			}
		}
		break;

		case(TOKEN_SYMBOL):{									//SYMBOL
			if(lex->last_id[--lex->position] != '/'){
				lex->position++;
				if(IsSym(lex->newchar)){
					lex->last_id[lex->position++] = lex->newchar;
					lex->row++;
				}
				else if(isalpha(lex->newchar) || lex->newchar == '_'){
					printLex(lex);
					lex->last_token_type = TOKEN_IDENTIFIER;
					lex->last_id[lex->position++] = lex->newchar;
					lex->row++;
				}
				else if(IsVacant(lex->newchar)){
					printLex(lex);
					if(lex->newchar == '\n'){
						lex->line++;
						lex->row = 0;
					}
					else
						lex->row++;
				}
				else if(isdigit(lex->newchar)){
					printLex(lex);
					lex->last_token_type = TOKEN_NUMBER;
					lex->last_id[lex->position++] = lex->newchar;
					lex->row++;
				}
			}
			else{
				lex->position++;
				if(PL0Lex_get_token(lex) == '/'){
					while(lex->newchar != '\n' && lex->newchar != EOF){
						PL0Lex_get_token(lex);
					}
					lex->last_token_type = TOKEN_NULL;
					lex->position = 0;
					for(int i=0; i<MAX_ID_LEN; i++)
						lex->last_id[i]='\0';
					lex->line++;
					lex->row = 0;
				}
				else if(lex->newchar == '*'){
					PL0Lex_get_token(lex);
					while(!(lex->newchar == '*' && PL0Lex_get_token(lex) == '/')){
						PL0Lex_get_token(lex);
						lex->last_token_type = TOKEN_NULL;
						lex->position = 0;
						for(int i=0; i<MAX_ID_LEN; i++)
							lex->last_id[i] = '\0';
						lex->line++;
						lex->row = 0;
					}
				}
				else{
					lex->last_id[lex->position] = '/';
					if(IsSym(lex->newchar)){
						lex->last_id[lex->position++] =lex->newchar;
						lex->row++;
					}
					else if(isalpha(lex->newchar) || lex->newchar =='_'){
						printLex(lex);
						lex->last_token_type = TOKEN_IDENTIFIER;
						lex->last_id[lex->position++] = lex->newchar;
						lex->row++;
					}
					else if(IsVacant(lex->newchar)){
						printLex(lex);
						if(lex->newchar == '\n'){
							lex->line++;
							lex->row = 0;
						}
						else
							lex->row++;
					}
					else if(isdigit(lex->newchar)){
						printLex(lex);
						lex->last_token_type = TOKEN_NUMBER;
						lex->last_id[lex->position++] = lex->newchar;
						lex->row++;
					}
					else
						;
				}
			}
		}
		break;

	}
}