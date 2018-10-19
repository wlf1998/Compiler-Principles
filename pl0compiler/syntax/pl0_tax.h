#ifndef __PL0_TAX_H__
#define __PL0_TAX_H__

#define TXMAX				500 		// max length of token table
#define MAXADDRESS 	32767		// max address

/*variables for token table*/
int table_index = 0;

/*struct for token table*/
typedef struct {
	char name[MAX_ID_LEN + 1];
	int kind;
	int value;
	short level;
	short address;
} Table;

/*definition for token table*/
Table token_table[TXMAX];

/*identifier type*/
enum idtype {
	ID_CONSTANT,
	ID_VARIABLE,
	ID_PROCEDURE
};


/*operations for token table*/
void table_append(PL0Lex * lex, int kind);

/*functions for syntax analysis*/
void statement(PL0Lex * lex); //analysis the statement
void condition(PL0Lex * lex); //analysis the condition statement
void expression(PL0Lex * lex); //analysis the expression
void term(PL0Lex * lex); //analysis the term
void factor(PL0Lex * lex); //analysis the factor
void program_block(PL0Lex * lex); //analysis the program block

#endif //pl0_tax.h