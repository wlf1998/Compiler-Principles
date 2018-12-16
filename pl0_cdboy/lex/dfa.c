#include<ctype.h>
#include<stdlib.h>
#include<stdio.h>
#include"pl0_lex.h"
#include"../common/common.h"
#include"../common/cdfilehelper.h"
#include"dfa.h"

BOOL Validate_azAZ_(char input, char param)
{
	return (input == '_' || isalpha(input)) == 0 ? FALSE : TRUE;
}

BOOL Validate_09(char input, char param)
{
	return isdigit(input) ? TRUE : FALSE;
}

BOOL Validate_azAZ__09(char input, char param)
{
	return (TRUE == Validate_azAZ_(input, '\0') || TRUE == Validate_09(input, '\0')) ? TRUE : FALSE;
}

BOOL Validate_Character(char input, char param)
{
	return input == param ? TRUE : FALSE;
}

DFA* DFA_Load(const char* filename)
{
	DFA* dfa = (DFA*)calloc(1, sizeof(DFA));
	Cdfh fh;
	cdfh_OpenRead(filename, &fh);
	int i;
	// char c;

	dfa->is_avaliable = TRUE;
	dfa->has_found_token = FALSE;
	dfa->current_state = 0;

	cdfh_ReadInt(&fh, 2, &i);
	dfa->state_number = i;

	dfa->states = (DFAState*)calloc(dfa->state_number, sizeof(DFAState));
	for (int j = 0; j < dfa->state_number; j++)
	{
		DFAState* s = (dfa->states) + j;

		cdfh_ReadInt(&fh, 2, &(s->state_id));

		cdfh_ReadInt(&fh, 1, &i);
		s->is_accepted_state = 1 == i ? TRUE : FALSE;

		cdfh_ReadInt(&fh, 3, &i);
		s->target_token_type = (PL0TokenType)i;

		cdfh_ReadInt(&fh, 2, &(s->validator_number));

		s->validators = (BOOL(**)(char input, char param))calloc(s->validator_number, sizeof(BOOL(*)(char input, char param)));
		s->params = (char*)calloc(s->validator_number, sizeof(char));
		s->target_states = (int*)calloc(s->validator_number, sizeof(int));
		for (int k = 0; k < s->validator_number; k++)
		{
			cdfh_ReadInt(&fh, 2, &i);
			switch (i)
			{
			case 0:
				s->validators[k] = Validate_azAZ_;
				break;

			case 1:
				s->validators[k] = Validate_09;
				break;

			case 2:
				s->validators[k] = Validate_azAZ__09;
				break;

			case 3:
				s->validators[k] = Validate_Character;
				break;

			default:
				break;
			}

			cdfh_ReadChar(&fh, s->params + k);

			cdfh_ReadInt(&fh, 2, s->target_states + k);
		}
	}

	return dfa;
}

DFAList* DFAList_Load()
{
	DFAList* list = (DFAList*)calloc(1, sizeof(DFAList));
	
	list->DFA_number = 4;
	list->DFAs = (DFA**)calloc(list->DFA_number, sizeof(DFA*));

	list->DFAs[0] = DFA_Load("lex/DFA_ID");
	list->DFAs[1] = DFA_Load("lex/DFA_NUM_OR_NID");
	list->DFAs[2] = DFA_Load("lex/DFA_SYMBOL");
	list->DFAs[3] = DFA_Load("lex/DFA_RESWORD");

	return list;
}

void DFAList_Validate(DFAList* list, BOOL* has_found_token, PL0TokenType* token_found)
{
	int dfa_num = list->DFA_number;

	// 检查已经找到 TOKEN 的优先级最大的 DFA
	int dfa_found_token = 0;
	int dfa_avaliable_num = 0;
	for (int i = 0; i < dfa_num; i++)
	{
		DFA* dfa = list->DFAs[i];
		if (TRUE == dfa->has_found_token)
		{
			*has_found_token = TRUE;
			dfa_found_token = i;
		}

		if (TRUE == dfa->is_avaliable)
		{
			dfa_avaliable_num++;
		}
	}

	if (TRUE == *has_found_token)
	{
		// 如果已经找到 TOKEN
		DFA* the_dfa = list->DFAs[dfa_found_token];
		*token_found = the_dfa->token_found;
	}
	else
	{
		// 如果未找到 TOKEN：
		// 检查有没有有效的 DFA，如果没有则找到 TOKEN_NULL
		if (dfa_avaliable_num > 0)
		{
			*has_found_token = FALSE;
		}
		else
		{
			*has_found_token = TRUE;
			*token_found = TOKEN_NULL;
		}
	}

	return;
}

void DFA_Reset(DFA* dfa)
{
	dfa->is_avaliable = TRUE;
	dfa->has_found_token = FALSE;
	dfa->current_state = 0;

	return;
}

void DFAList_Reset(DFAList* list)
{
	int dfa_num = list->DFA_number;
	for (int i = 0; i < dfa_num; i++)
	{
		DFA_Reset(list->DFAs[i]);
	}
}

void DFA_Input(DFA* dfa, char c)
{
	DFAState* s = dfa->states + dfa->current_state;
	BOOL can_accept_input = FALSE;

	// 检查对输入字符 c, 是否能接受输入
	int vnum = s->validator_number;
	int v;
	for (v = 0; v < vnum; v++)
	{
		if (TRUE == (s->validators[v])(c, s->params[v]))
		{
			can_accept_input = TRUE;
			break;
		}
	}

	if (TRUE == can_accept_input)
	{
		// 能：切换到下个状态
		dfa->current_state = s->target_states[v];
	}
	else
	{
		// 否：先使该 DFA 无效，再检查该状态是否为接受状态
		dfa->is_avaliable = FALSE;
		if (TRUE == s->is_accepted_state)
		{
			// 是：已经找到 TOKEN
			dfa->token_found = s->target_token_type;
			dfa->has_found_token = TRUE;
		}
	}
}
