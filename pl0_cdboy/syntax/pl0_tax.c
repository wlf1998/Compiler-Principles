#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/common.h"
#include "../lex/pl0_lex.h"
#include "pl0_tax.h"

void table_append(PL0Lex * lex, int kind) 
{
	strcpy_s(token_table[table_index].name, MAX_ID_LEN + 1, lex->last_id);
	token_table[table_index].kind = kind;
	switch (kind) {
	case ID_CONSTANT:
		if (lex->last_num > MAXADDRESS) {
			printf_s("the number is too great!\n");
			lex->last_num = 0;
		}
		token_table[table_index].value = lex->last_num;
		token_table[table_index].level = lex->last_level;
		break;
	case ID_VARIABLE:
		token_table[table_index].level = lex->last_level;
		break;
	case ID_PROCEDURE:
		token_table[table_index].level = lex->last_level;
		break;
	}
	table_index += 1;
}

void PrintTaxstack(char* str)
{
	printf_s(_taxstack);
	printf_s(str);
}

void PushTaxstack(char* str)
{
	strcpy_s(_taxstack + strlen(_taxstack), strlen(str) + 1, str);
}

void PopTaxstack(int poplength)
{
	_taxstack[strlen(_taxstack) - poplength] = '\0';
}

BOOL ValidateProgramBlockBeginning(PL0Lex* lex)
{
	if (lex->last_token_type == TOKEN_CONST
		|| lex->last_token_type == TOKEN_VAR
		|| lex->last_token_type == TOKEN_PROCEDURE
		|| TOKEN_IDENTIFIER == lex->last_token_type
		|| TOKEN_CALL == lex->last_token_type
		|| TOKEN_BEGIN == lex->last_token_type
		|| TOKEN_IF == lex->last_token_type
		|| TOKEN_WHILE == lex->last_token_type)
		return TRUE;
	else
		return FALSE;
}

BOOL ValidateExpressionBeginning(PL0Lex* lex)
{
	return ValidateFactorBeginning(lex);
}

BOOL ValidateTermBeginning(PL0Lex* lex)
{
	return ValidateFactorBeginning(lex);
}

BOOL ValidateFactorBeginning(PL0Lex* lex)
{
	if (TOKEN_IDENTIFIER == lex->last_token_type
		|| TOKEN_NUMBER == lex->last_token_type
		|| TOKEN_MINUS == lex->last_token_type
		|| TOKEN_LPAREN == lex->last_token_type)
		return TRUE;
	else
		return FALSE;
}

BOOL ValidateStatementBeginning(PL0Lex* lex)
{
	if (TOKEN_IDENTIFIER == lex->last_token_type 
		|| TOKEN_CALL == lex->last_token_type 
		|| TOKEN_BEGIN == lex->last_token_type 
		|| TOKEN_IF == lex->last_token_type 
		|| TOKEN_WHILE == lex->last_token_type)
		return TRUE;
	else
		return FALSE;
}

// 调用该函数前，需先让词法分析器取一个单元
void Program(PL0Lex* lex)
{
	table_index = 0;
	lex->last_level = 0;
	_taxstack[0] = '\0';
	PrintTaxstack("S\n");
	if (TRUE == ValidateProgramBlockBeginning(lex))
	{
		PrintTaxstack(". PB\n");
		PushTaxstack(". ");
		program_block(lex);
		PopTaxstack(2);
		if (TOKEN_PERIOD == lex->last_token_type)
		{
			PrintTaxstack("$\n");
		}
		else
			printf_s("There must be a '.' as the end of a program\n");
	}
	else
		printf_s("Invalid beginning for a program\n");
}

void program_block(PL0Lex * lex) 
{
	printf_s("Trying to analyse a program block...\n");
	// PL0Lex_get_token(lex);
	PushTaxstack("PB ");
	PrintTaxstack("\n");

	while (1)
	{
		if (TOKEN_CONST == lex->last_token_type)
		{
			PushTaxstack("; ");
			PrintTaxstack("CONST_B const\n");
			ConstB(lex);
			PopTaxstack(2);
			if (TOKEN_SEMICOLON == lex->last_token_type)
			{
				PrintTaxstack("\n");
				PL0Lex_get_token(lex);
			}
			else
				printf_s("There must be a ';' following the definition\n");
		}
		else if (TOKEN_VAR == lex->last_token_type)
		{
			PushTaxstack("; ");
			PrintTaxstack("VAR_B var\n");
			VarB(lex);
			PopTaxstack(2);
			if (TOKEN_SEMICOLON == lex->last_token_type)
			{
				PrintTaxstack("\n");
				PL0Lex_get_token(lex);
			}
			else
				printf_s("There must be a ';' following the definition\n");
		}
		else if (TOKEN_PROCEDURE == lex->last_token_type)
		{
			PushTaxstack("; ");
			PrintTaxstack("PROCEDURE_B procedure\n");
			ProcedureB(lex);
			PopTaxstack(2);
			if (TOKEN_SEMICOLON == lex->last_token_type)
			{
				PrintTaxstack("\n");
				PL0Lex_get_token(lex);
			}
			else
				printf_s("There must be a ';' following the definition\n");
		}
		else if (TRUE == ValidateStatementBeginning(lex))
		{
			statement(lex);
		}
		else
		{
			PopTaxstack(3);
			PrintTaxstack("\n");
			// PL0Lex_get_token(lex);
			break;
		}
	}
}

void ConstB(PL0Lex * lex)
{
	int flag = 1;
	while (1 == flag)
	{
		flag = 0;
		PrintTaxstack("CONST_B\n");
		PL0Lex_get_token(lex);
		if (TOKEN_IDENTIFIER == lex->last_token_type)
		{
			PrintTaxstack("CONST_B2 number = id\n");
			PrintTaxstack("CONST_B2 number =\n");
			PL0Lex_get_token(lex);
			if (TOKEN_EQU == lex->last_token_type)
			{
				PrintTaxstack("CONST_B2 number\n");
				PL0Lex_get_token(lex);
				if (TOKEN_NUMBER == lex->last_token_type)
				{
					table_append(lex, ID_CONSTANT);
					PrintTaxstack("CONST_B2\n");
					PL0Lex_get_token(lex);
					if (TOKEN_COMMA == lex->last_token_type)
					{
						PrintTaxstack("CONST_B ,\n");
						flag = 1;
					}
					else
					{
						PrintTaxstack("\n");
					}
				}
				else
					printf_s("There must be a number following the '='");
			}
			else if (TOKEN_BECOMES == lex->last_token_type)
				printf_s("Found ':=' when expecting '='\n");
			else
				printf_s("There must be an '=' followint the identifier\n");
		}
		else
			printf_s("There must be an identifier following 'const'\n");
	}
}

void VarB(PL0Lex* lex)
{
	int flag = 1;
	while (1 == flag)
	{
		flag = 0;
		PrintTaxstack("VAR_B\n");
		PL0Lex_get_token(lex);
		if (TOKEN_IDENTIFIER == lex->last_token_type)
		{
			table_append(lex, ID_VARIABLE);
			PrintTaxstack("VAR_B2 id\n");
			PrintTaxstack("VAR_B2\n");
			PL0Lex_get_token(lex);
			if (TOKEN_COMMA == lex->last_token_type)
			{
				PrintTaxstack("VAR_B ,\n");
				flag = 1;
			}
			else
			{
				PrintTaxstack("\n");
			}
		}
		else
			printf_s("There must be an identifier following 'var'\n");
	}
}

void ProcedureB(PL0Lex* lex)
{
	PrintTaxstack("PROCEDURE_B\n");
	PL0Lex_get_token(lex);
	if (TOKEN_IDENTIFIER == lex->last_token_type)
	{
		table_append(lex, ID_PROCEDURE);
		PrintTaxstack("PB ; id\n");
		PrintTaxstack("PB ;\n");
		PL0Lex_get_token(lex);
		if (TOKEN_SEMICOLON == lex->last_token_type)
		{
			PrintTaxstack("PB\n");
			PL0Lex_get_token(lex);
			if (ValidateProgramBlockBeginning(lex) == TRUE)
			{
				lex->last_level++;
				program_block(lex);
				lex->last_level--;
				/*
				if (TOKEN_SEMICOLON == lex->last_token_type)
				{
					PopTaxstack(2);
					PrintTaxstack("\n");
					PL0Lex_get_token(lex);
				}
				else
					printf_s("There must be a ';' following the program block\n");
				*/
			}
			else
				printf_s("There must be a program block following the ';'\n");
		}
		else
			printf_s("There must be a ';' following the identifier\n");
	}
	else
		printf_s("There must be an identifier following 'procedure'\n");
}

// 调用本函数时，不一定已检测到合法 TOKEN
void Statements(PL0Lex* lex)
{
	int flag = 0; // 标识是否至少检测到了一个语句
	while (TRUE == ValidateStatementBeginning(lex))
	{
		PrintTaxstack("STATEMENTS ; STATEMENT\n");
		PushTaxstack("STATEMENTS ; ");
		statement(lex);
		PopTaxstack(13);
		flag = 1;
		if (TOKEN_SEMICOLON == lex->last_token_type)
		{
			PrintTaxstack("STATEMENTS\n");
			PL0Lex_get_token(lex);
		}
		else
			printf_s("There must be a ';' following the statement\n");
	}
	
	if (0 == flag)
	{
		printf_s("There must be one statement in the statements at least\n");
	}
	else
	{
		PrintTaxstack("\n");
	}
}

// 调用本函数时，不一定已检测到合法 TOKEN
void statement(PL0Lex * lex) 
{
	printf_s("Trying to analyse a statement...\n");
	if (TOKEN_IDENTIFIER == lex->last_token_type)
	{
		PrintTaxstack("EXPRESSION := id\n");
		PrintTaxstack("EXPRESSION :=\n");
		PL0Lex_get_token(lex);
		if (TOKEN_BECOMES == lex->last_token_type)
		{
			PrintTaxstack("EXPRESSION\n");
			PL0Lex_get_token(lex);
			expression(lex);
		}
		else
			printf_s("There must be a ':=' following the identifier\n");
	}
	else if (TOKEN_CALL == lex->last_token_type)
	{
		PrintTaxstack("id call\n");
		PrintTaxstack("id\n");
		PL0Lex_get_token(lex);
		if (TOKEN_IDENTIFIER == lex->last_token_type)
		{
			PrintTaxstack("\n");
			PL0Lex_get_token(lex);
		}
		else
			printf_s("There must be an identifier following 'call'\n");
	}
	else if (TOKEN_BEGIN == lex->last_token_type)
	{
		PrintTaxstack("end STATEMENTS begin\n");
		PrintTaxstack("end STATEMENTS");
		PL0Lex_get_token(lex);
		PushTaxstack("end ");
		Statements(lex);
		PopTaxstack(4);
		if (TOKEN_END == lex->last_token_type)
		{
			PrintTaxstack("\n");
			PL0Lex_get_token(lex);
		}
		else
			printf_s("There must be an 'end' following the statements\n");
	}
	else if (TOKEN_IF == lex->last_token_type)
	{
		PrintTaxstack("STATEMENT then CONDITION if\n");
		PrintTaxstack("STATEMENT then CONDITION\n");
		PL0Lex_get_token(lex);
		PushTaxstack("STATEMENT then ");
		condition(lex);
		PopTaxstack(15);
		if (TOKEN_THEN == lex->last_token_type)
		{
			PrintTaxstack("STATEMENTS\n");
			PL0Lex_get_token(lex);
			statement(lex);
		}
		else
			printf_s("There must be a ';' following the statement\n");
	}
	else if (TOKEN_WHILE == lex->last_token_type)
	{
		PrintTaxstack("STATEMENT do CONDITION while\n");
		PrintTaxstack("STATEMENT do CONDITION\n");
		PL0Lex_get_token(lex);
		PushTaxstack("STATEMENT do ");
		condition(lex);
		PopTaxstack(13);
		if (TOKEN_DO == lex->last_token_type)
		{
			PrintTaxstack("STATEMENT\n");
			PL0Lex_get_token(lex);
			statement(lex);
		}
		else
			printf_s("There must be a 'do' following the condition\n");
	}
	else
	{
		printf_s("Invalid statement\n");
	}
}

// 调用本函数时，不一定已检测到合法 TOKEN
void condition(PL0Lex * lex)
{
	printf_s("Trying to analyse a condition...\n");

	if (TOKEN_ODD == lex->last_token_type)
	{
		PrintTaxstack("EXPRESSION odd\n");
		PrintTaxstack("EXPRESSION\n");
		PL0Lex_get_token(lex);
		expression(lex);
	}
	else if (TRUE == ValidateExpressionBeginning(lex))
	{
		PrintTaxstack("EXPRESSION op EXPRESSION\n");
		PushTaxstack("EXPRESSION op ");
		expression(lex);
		PopTaxstack(14);
		PrintTaxstack("EXPRESSION op\n");
		if (TOKEN_EQU == lex->last_token_type
			|| TOKEN_NEQ == lex->last_token_type
			|| TOKEN_LES == lex->last_token_type
			|| TOKEN_GTR == lex->last_token_type
			|| TOKEN_LEQ == lex->last_token_type
			|| TOKEN_GEQ == lex->last_token_type)
		{
			PrintTaxstack("EXPRESSION\n");
			PL0Lex_get_token(lex);
			expression(lex);
		}
		else
			printf_s("There must be a relationship operator following the expression\n");
	}
	else
	{
		printf_s("There must be an 'odd' or an expression as a start of a condition\n");
	}
}

// 调用本函数时，不一定已检测到合法 TOKEN
void expression(PL0Lex * lex)
{
	printf_s("Trying to analyse an expression...\n");

	if (TRUE == ValidateExpressionBeginning(lex))
	{
		PrintTaxstack("TERM_B TERM\n");
		PushTaxstack("TERM_B ");
		term(lex);
		PopTaxstack(7);

		int flag = 1;
		while (1 == flag)
		{
			flag = 0;
			
			if (TOKEN_PLUS == lex->last_token_type || TOKEN_MINUS == lex->last_token_type)
			{
				if (TOKEN_PLUS == lex->last_token_type)
					PrintTaxstack("TERM_B TERM +\n");
				else
					PrintTaxstack("TERM_B TERM -\n");
				PrintTaxstack("TERM_B TERM\n");
				PL0Lex_get_token(lex);
				PushTaxstack("TERM_B ");
				term(lex);
				PopTaxstack(7);
				flag = 1;
			}
			else
			{
				PrintTaxstack("\n");
			}
		}
	}
	else
	{
		printf_s("There must be a term as a start of an expression\n");
	}
}

// 调用本函数时，不一定已检测到合法 TOKEN
void term(PL0Lex * lex)
{
	printf_s("Trying to analyse a term...\n");

	if (TRUE == ValidateTermBeginning(lex))
	{
		PrintTaxstack("FACTOR_B FACTOR\n");
		PushTaxstack("FACTOR_B ");
		factor(lex);
		PopTaxstack(9);

		int flag = 1;
		while (1 == flag)
		{
			flag = 0;

			if (TOKEN_TIMES == lex->last_token_type || TOKEN_SLASH == lex->last_token_type)
			{
				if (TOKEN_TIMES == lex->last_token_type)
					PrintTaxstack("TERM_B TERM *\n");
				else
					PrintTaxstack("TERM_B TERM /\n");
				PrintTaxstack("FACTOR_B FACTOR\n");
				PL0Lex_get_token(lex);
				PushTaxstack("FACTOR_B ");
				factor(lex);
				PopTaxstack(9);
				flag = 1;
			}
			else
			{
				PrintTaxstack("\n");
			}
		}
	}
	else
		printf_s("There must be a factor as a start of a term.\n");
}

// 调用本函数时，不一定已检测到合法 TOKEN
void factor(PL0Lex * lex)
{
	printf_s("Trying to analyse a factor...\n");

	if (TOKEN_IDENTIFIER == lex->last_token_type || TOKEN_NUMBER == lex->last_token_type)
	{
		PrintTaxstack("\n");
		PL0Lex_get_token(lex);
	}
	else if (TOKEN_MINUS == lex->last_token_type)
	{
		PrintTaxstack("EXPRESSION -\n");
		PrintTaxstack("EXPRESSION\n");
		PL0Lex_get_token(lex);
		expression(lex);
	}
	else if (TOKEN_LPAREN == lex->last_token_type)
	{
		PrintTaxstack(") EXPRESSION (\n");
		PrintTaxstack(") EXPRESSION\n");
		PL0Lex_get_token(lex);
		PushTaxstack(") ");
		expression(lex);
		PopTaxstack(2);
		if (TOKEN_RPAREN == lex->last_token_type)
		{
			PrintTaxstack("\n");
			PL0Lex_get_token(lex);
		}
		else
			printf_s("There must be a ')' following the expression\n");
	}
	else
	{
		printf_s("There must be an id, a number, a '-' or a '(' as the start of a factor\n");
	}
}
