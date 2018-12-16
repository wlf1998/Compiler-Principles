#include <stdlib.h>
#include<string.h>
#include "../common/common.h"
#include "../common/cdfilehelper.h"
#include "pl0_lex.h"
#include "dfa.h"

/* Global variables */
extern FILE * fin;

const char * TOKEN_RESERVED_WORDS[NRW] = {"var", "const", "procedure", "begin", "end", "if", "then", "do", "while", "call", "odd"};
const char * TOKEN_SYMBOLS[NSYM] = { "+", "-", "*", "/", "=", "!=", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":=" };

PL0Lex * PL0Lex_create(PL0Compiler * parent)
{
	PL0Lex * lex = (PL0Lex *) calloc(1, sizeof(PL0Lex));
	lex->compiler = parent;
	lex->filehelper = (Cdfh*)calloc(1, sizeof(Cdfh));
	cdfh_GetOpenedFile(lex->filehelper, fin, 1, 0);
	lex->dfas = DFAList_Load();

	return lex;
}

BOOL PL0Lex_destroy(PL0Lex * lex)
{
	DFA** list = lex->dfas->DFAs;
	for (int i = 0; i < lex->dfas->DFA_number; i++)
	{
		DFA* dfa = list[i];
		DFAState* states = dfa->states;
		for (int j = 0; j < dfa->state_number; j++)
		{
			DFAState* state = states + j;
			free(state->validators);
			free(state->params);
			free(state->target_states);
			free(state);
		}
		free(states);
		free(dfa);
	}
	free(list);

	free(lex->filehelper);
	free(lex);
	return TRUE;
}

void PL0Lex_resolve_token(PL0Lex* lex, PL0TokenType token, char* str_read, int str_read_len)
{
	str_read[str_read_len] = '\0';

	switch (token)
	{
	case TOKEN_IDENTIFIER:
		lex->last_token_type = TOKEN_IDENTIFIER;
		strcpy_s(lex->last_id, str_read_len + 1, str_read);
		break;

	case TOKEN_NUMBER:
		lex->last_token_type = TOKEN_NUMBER;
		lex->last_num = atoi(str_read);
		break;

	case TOKEN_NULL:
		lex->last_token_type = TOKEN_NULL;
		strcpy_s(lex->last_id, str_read_len + 1, str_read);

	default:
		lex->last_token_type = token;
		break;
	}
}

BOOL PL0Lex_get_token(PL0Lex * lex)
{
	Cdfh* fh = lex->filehelper;
	BOOL has_found_token = FALSE;
	PL0TokenType token_found;

	int start_row = fh->current_row;                       // 该 TOKEN 起始行
	int start_column = fh->current_column;                 // 起始列
	BOOL has_start_record_position = FALSE;                // 标识是否已经开始记录当前 TOKEN 的位置信息

	char str_read[MAX_ID_LEN + 1];                         // 目前已经读入的所有字符
	int str_read_len = 0;                                  // 目前已经读入的字符数

	char last_input = ' ';
	char input = ' ';
	BOOL has_just_seek_minus1 = FALSE;                     // 标识是否刚刚调用了 cdfh_SeekMinus1() 函数
	char ws[] = { ' ', '\t', '\r', '\n' };

	int dfa_num = lex->dfas->DFA_number;
	DFAList_Reset(lex->dfas);

	while (!has_found_token)
	{
		// 这里为什么要判断刚刚有没有回退？
		// 试想这种情形：
		// a;//
		// 读出第一个 / 之后会回退，
		// 如果直接将 last_input 赋值为 /
		// 那么将直接构成单行注释！
		if (FALSE == has_just_seek_minus1)
		{
			last_input = input;
		}
		else
		{
			has_just_seek_minus1 = FALSE;
		}

		// 读一个字符
		input = cdfh_GetC(fh);
		if (EOF == input)
		{
			return FALSE;
		}

		// 读到空白：如果前一个字符不是空白，则无需操作；否则跳过这个空白
		if (cdfh_IsWhiteSpace(input, ws, 4) && cdfh_IsWhiteSpace(last_input, ws, 4))
		{
			continue;
		}

		// 读到单行注释
		if ('/' == last_input && '/' == input)
		{
			// 重置所有 DFA 状态
			DFAList_Reset(lex->dfas);

			// 读到换行符为止
			while ('\n' != input)
			{
				input = cdfh_GetC(fh);
			}

			// 这里为什么要改成空格？
			// 如果不是空格而下个字符是空白的话，
			// 那就不会跳过那个空白
			// 就会产生一个 TOKEN_NULL
			input = ' ';

			continue;
		}

		// 读到多行注释
		if ('/' == last_input && '*' == input)
		{
			// 重置所有 DFA 状态
			DFAList_Reset(lex->dfas);

			// 先读一个字符，防止 /*/ 出现
			last_input = input;
			input = cdfh_GetC(fh);

			// 读到结束符为止
			while ('*' != last_input || '/' != input)
			{
				last_input = input;
				input = cdfh_GetC(fh);
			}

			input = ' ';

			continue;
		}

		// 记录起始位置信息
		if (FALSE == has_start_record_position)
		{
			start_row = fh->current_row;
			start_column = fh->last_column;
			has_start_record_position = TRUE;
		}

		// 将该字符输入所有有效的 DFA
		for (int i = 0; i < dfa_num; i++)
		{
			DFA* dfa = lex->dfas->DFAs[i];
			if (TRUE == dfa->is_avaliable)
			{
				DFA_Input(dfa, input);
			}
		}

		// 检查是否已经找到 TOKEN
		DFAList_Validate(lex->dfas, &has_found_token, &token_found);
		if (TRUE == has_found_token)
		{
			// 是
			PL0Lex_resolve_token(lex, token_found, str_read, str_read_len);

			// 返回位置信息
			lex->last_start_column = start_column;
			lex->last_start_row = start_row;
			lex->last_len = str_read_len;
			has_start_record_position = FALSE;

			// 重置所有 DFA 状态
			DFAList_Reset(lex->dfas);

			// 文件指针回退一个字符
			cdfh_SeekMinus1(fh);
			has_just_seek_minus1 = TRUE;
		}
		else
		{
			// 否：继续
			str_read[str_read_len++] = input;
			continue;
		}
	}

	return TRUE;
}
