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

	int start_row = fh->current_row;                       // �� TOKEN ��ʼ��
	int start_column = fh->current_column;                 // ��ʼ��
	BOOL has_start_record_position = FALSE;                // ��ʶ�Ƿ��Ѿ���ʼ��¼��ǰ TOKEN ��λ����Ϣ

	char str_read[MAX_ID_LEN + 1];                         // Ŀǰ�Ѿ�����������ַ�
	int str_read_len = 0;                                  // Ŀǰ�Ѿ�������ַ���

	char last_input = ' ';
	char input = ' ';
	BOOL has_just_seek_minus1 = FALSE;                     // ��ʶ�Ƿ�ոյ����� cdfh_SeekMinus1() ����
	char ws[] = { ' ', '\t', '\r', '\n' };

	int dfa_num = lex->dfas->DFA_number;
	DFAList_Reset(lex->dfas);

	while (!has_found_token)
	{
		// ����ΪʲôҪ�жϸո���û�л��ˣ�
		// �����������Σ�
		// a;//
		// ������һ�� / ֮�����ˣ�
		// ���ֱ�ӽ� last_input ��ֵΪ /
		// ��ô��ֱ�ӹ��ɵ���ע�ͣ�
		if (FALSE == has_just_seek_minus1)
		{
			last_input = input;
		}
		else
		{
			has_just_seek_minus1 = FALSE;
		}

		// ��һ���ַ�
		input = cdfh_GetC(fh);
		if (EOF == input)
		{
			return FALSE;
		}

		// �����հף����ǰһ���ַ����ǿհף������������������������հ�
		if (cdfh_IsWhiteSpace(input, ws, 4) && cdfh_IsWhiteSpace(last_input, ws, 4))
		{
			continue;
		}

		// ��������ע��
		if ('/' == last_input && '/' == input)
		{
			// �������� DFA ״̬
			DFAList_Reset(lex->dfas);

			// �������з�Ϊֹ
			while ('\n' != input)
			{
				input = cdfh_GetC(fh);
			}

			// ����ΪʲôҪ�ĳɿո�
			// ������ǿո���¸��ַ��ǿհ׵Ļ���
			// �ǾͲ��������Ǹ��հ�
			// �ͻ����һ�� TOKEN_NULL
			input = ' ';

			continue;
		}

		// ��������ע��
		if ('/' == last_input && '*' == input)
		{
			// �������� DFA ״̬
			DFAList_Reset(lex->dfas);

			// �ȶ�һ���ַ�����ֹ /*/ ����
			last_input = input;
			input = cdfh_GetC(fh);

			// ����������Ϊֹ
			while ('*' != last_input || '/' != input)
			{
				last_input = input;
				input = cdfh_GetC(fh);
			}

			input = ' ';

			continue;
		}

		// ��¼��ʼλ����Ϣ
		if (FALSE == has_start_record_position)
		{
			start_row = fh->current_row;
			start_column = fh->last_column;
			has_start_record_position = TRUE;
		}

		// �����ַ�����������Ч�� DFA
		for (int i = 0; i < dfa_num; i++)
		{
			DFA* dfa = lex->dfas->DFAs[i];
			if (TRUE == dfa->is_avaliable)
			{
				DFA_Input(dfa, input);
			}
		}

		// ����Ƿ��Ѿ��ҵ� TOKEN
		DFAList_Validate(lex->dfas, &has_found_token, &token_found);
		if (TRUE == has_found_token)
		{
			// ��
			PL0Lex_resolve_token(lex, token_found, str_read, str_read_len);

			// ����λ����Ϣ
			lex->last_start_column = start_column;
			lex->last_start_row = start_row;
			lex->last_len = str_read_len;
			has_start_record_position = FALSE;

			// �������� DFA ״̬
			DFAList_Reset(lex->dfas);

			// �ļ�ָ�����һ���ַ�
			cdfh_SeekMinus1(fh);
			has_just_seek_minus1 = TRUE;
		}
		else
		{
			// �񣺼���
			str_read[str_read_len++] = input;
			continue;
		}
	}

	return TRUE;
}
