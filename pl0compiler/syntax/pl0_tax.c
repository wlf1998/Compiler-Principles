#include <stdio.h>
#include <stdlib.h>
#include "../common/common.h"
#include "../lex/pl0_lex.h"
#include "pl0_tax.h"

/*operations for token table*/
void table_append(PL0Lex * lex, int kind) {
	strcpy(token_table[table_index].name, lex->last_id);
	token_table[table_index].kind = kind;
	switch (kind) {
		case ID_CONSTANT:
			if (lex->last_num > MAXADDRESS) {
				printf("the number is too great!\n");
				lex->last_num = 0;
			}
			token_table[table_index].value = lex->last_num;
			break;
		case ID_VARIABLE:
			token_table[table_index].level = lex->last_level;
			break;
		case ID_PROCEDURE:
			token_table[table_index].level = lex->last_level;
			break;
	} //switch
	table_index += 1;
} //table_append

/*functions for syntax analysis*/
void statement(PL0Lex * lex) {
	printf("analysis the statement\n");
}

void condition(PL0Lex * lex) {
	printf("analysis the condition expression\n");
}

void expression(PL0Lex * lex) {
	printf("analysis the expression\n");
}

void term(PL0Lex * lex) {
	printf("analysis the term\n");
}

void factor(PL0Lex * lex) {
	printf("analysis the factor\n");
}




void const_declaration(PL0Lex * lex) {
	if (lex->last_token_type == TOKEN_IDENTIFIER) {
		PL0Lex_get_token(lex);
		if (lex->last_token_type == TOKEN_EQU || lex->last_token_type == TOKEN_BECOMES) {
			if (lex->last_token_type == TOKEN_BECOMES)
				printf("found ':=' when expecting '='\n");
			PL0Lex_get_token(lex);
			if (lex->last_token_type == TOKEN_NUMBER) {
				table_append(lex, ID_CONSTANT);
				PL0Lex_get_token(lex);
			} else {
				printf("there must be a number to follow '='\n");
			}
		} else {
			printf("there must be an '=' to follow the identifier\n");
		}
	} else {
		printf("There must be an identifier to follow 'const'\n");
	}
} //const_declaration

void program_block(PL0Lex * lex) {
	printf("analysis the program block\n");
	// PL0Lex * lex = (PL0Lex *) calloc(1, sizeof(PL0Lex));
	// PL0Lex_get_token(lex);
	do {
		if (lex->last_token_type == TOKEN_CONST) {
			PL0Lex_get_token(lex);
			do {
				const_declaration(lex);
				while (lex->last_token_type == TOKEN_COMMA) {
					PL0Lex_get_token(lex);
					const_declaration(lex);
				}
				if (lex->last_token_type == TOKEN_SEMICOLON) {
					PL0Lex_get_token(lex);
				}
				else {
					printf("missing ',' or ';'\n");
				}
			} while (lex->last_token_type == TOKEN_IDENTIFIER);
		}
	} while(lex->last_token_type == TOKEN_CONST || lex->last_token_type == TOKEN_VAR || lex->last_token_type == TOKEN_PROCEDURE || lex->last_token_type == TOKEN_NULL);
} //program_block
