#pragma once
#ifndef DFA_H
#define DFA_H

typedef struct _PL0DFA_state
{
	int state_id;                                        // 状态 ID
	BOOL is_accepted_state;                              // 标识该状态是否为一个接受状态
	PL0TokenType target_token_type;                      // 如果它是一个接受状态，标识目标词法单元
	int validator_number;                                // 验证器个数
	BOOL(**validators)(char input, char param);          // 验证器数组，数组元素类型：函数指针
	char* params;                                        // 如果验证器需要一个额外参数，那么将这个数组内的对应元素传进去
	int* target_states;                                  // 与验证器数组配套的目标状态数组
} DFAState;

typedef struct _PL0DFA
{
	BOOL is_avaliable;                                   // 指示该 DFA 当前是否有效
	BOOL has_found_token;                                // 指示该 DFA 是否已经找到一个 TOKEN
	PL0TokenType token_found;                            // 标识已经找到的 TOKEN
	int current_state;
	int state_number;
	DFAState* states;                                    // 状态数组，其中状态所拥有的索引值即为索引 ID
} DFA;

typedef struct _PL0DFAList
{
	int DFA_number;
	DFA** DFAs;
} DFAList;

BOOL Validate_azAZ_(char input, char param);

BOOL Validate_09(char input, char param);

BOOL Validate_azAZ__09(char input, char param);

BOOL Validate_Character(char input, char param);

DFA* DFA_Load(const char* filename);

DFAList* DFAList_Load();

// 检查是否已经找到了 TOKEN
void DFAList_Validate(DFAList* list, BOOL* has_found_token, PL0TokenType* token_found);

void DFA_Reset(DFA* dfa);

// 重置所有 DFA 的状态
void DFAList_Reset(DFAList* list);

void DFA_Input(DFA* dfa, char c);

#endif // !DFA_H
