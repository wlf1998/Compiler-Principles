#ifndef __PL0_TAX_H__
#define __PL0_TAX_H__

#define TXMAX				500 		// max length of token table
#define MAXADDRESS 	32767		// max address

/*variables for token table*/
int table_index;

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

// 语法分析栈中的已有字符
char _taxstack[1000];
void PrintTaxstack(char* str);
void PushTaxstack(char* str);
void PopTaxstack(int poplength);

/*operations for token table*/
void table_append(PL0Lex * lex, int kind);

/*functions for syntax analysis*/
void Program(PL0Lex* lex);
void program_block(PL0Lex * lex);
void ConstB(PL0Lex * lex);
void VarB(PL0Lex * lex);
void ProcedureB(PL0Lex * lex);
void statement(PL0Lex * lex);
void Statements(PL0Lex* lex);
void condition(PL0Lex * lex);
void expression(PL0Lex * lex);
void term(PL0Lex * lex);
void factor(PL0Lex * lex);

BOOL ValidateProgramBlockBeginning(PL0Lex* lex);
BOOL ValidateExpressionBeginning(PL0Lex* lex);
BOOL ValidateTermBeginning(PL0Lex* lex);
BOOL ValidateFactorBeginning(PL0Lex* lex);
BOOL ValidateStatementBeginning(PL0Lex* lex);

#endif //pl0_tax.h