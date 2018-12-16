#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "common/common.h"
#include "lex/pl0_lex.h"
#include "syntax/pl0_tax.h"


void show_help(const char * arg0)
{
	printf("Usage: %s <src>\n\n", arg0);
}

int main(int argc, char * argv[])
{
	// 检查参数
	if (argc != 3)
	{
		show_help(argv[0]);
		exit(-1);
	}

	// 打开源文件
	fopen_s(&fin, argv[1], "rb");
	if (!fin)
	{
		printf("Fatal: Cannot open file for reading: %s\n", argv[1]);
		exit(-1);
	}

	// 创建输出文件
	FILE* fout;
	fopen_s(&fout, argv[2], "w");
	if (!fout)
	{
		printf("Fatal: Cannot create file for writing: %s\n", argv[1]);
		exit(-1);
	}

	printf("=== This is the program for PL/0 testing ===\n");

	compiler = PL0Compiler_create();
	PL0Lex * lex = compiler->lex;

	// 词法分析
	/*
	printf("--- Lexical Analysis testing ---\n");
	PL0Lex * lex = compiler->lex;
	while (PL0Lex_get_token(lex))
	{
		// assert(lex->last_token_type != TOKEN_NULL);
		assert(lex->last_token_type != TOKEN_RESWORDS);
		assert(lex->last_token_type != TOKEN_SYMBOL);

		if (lex->last_token_type == TOKEN_NULL)
		{
			fprintf_s(fout, "Unknown word:         %-10s  %-3d : %-3d - %-3d\n",
				lex->last_id,
				lex->last_start_row,
				lex->last_start_column,
				lex->last_start_column + lex->last_len - 1);
		}
		else if (lex->last_token_type == TOKEN_IDENTIFIER)
		{
			fprintf_s(fout, "Identifier:           %-10s  %-3d : %-3d - %-3d\n",
				lex->last_id,
				lex->last_start_row,
				lex->last_start_column,
				lex->last_start_column + lex->last_len - 1);
		}
		else if (lex->last_token_type == TOKEN_NUMBER)
		{
			char numstr[MAX_NUM_LEN + 1];
			_itoa_s(lex->last_num, numstr, MAX_NUM_LEN + 1, 10);

			fprintf_s(fout, "Number:           %-10s    %-3d : %-3d - %-3d\n",
				numstr,
				lex->last_start_row,
				lex->last_start_column,
				lex->last_start_column + lex->last_len - 1);
		}
		else if (lex->last_token_type > TOKEN_RESWORDS)
		{
			fprintf_s(fout, "Reserved word:  %-10s    %-3d : %-3d - %-3d\n",
				TOKEN_RESERVED_WORDS[(int)lex->last_token_type - (int)TOKEN_RESWORDS - 1],
				lex->last_start_row,
				lex->last_start_column,
				lex->last_start_column + lex->last_len - 1);
		}
		else
		{
			fprintf_s(fout, "Symbol:             %-10s   %-3d : %-3d - %-3d\n",
				TOKEN_SYMBOLS[(int)lex->last_token_type - (int)TOKEN_SYMBOL - 1],
				lex->last_start_row,
				lex->last_start_column,
				lex->last_start_column + lex->last_len - 1);
		}
	}

	printf("--- Lexical Analysis testing end. ---\n");
	*/

	/*test syntax ayalysis*/
	PL0Lex_get_token(lex);
	Program(lex);

	/* Cleanup */
	fclose(fin);

	/* Finish */
	printf("=== Normal end for testing. ===\n");
	system("pause");
	return 0;
	getchar();
}
