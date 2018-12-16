#pragma once
#ifndef DFA_H
#define DFA_H

typedef struct _PL0DFA_state
{
	int state_id;                                        // ״̬ ID
	BOOL is_accepted_state;                              // ��ʶ��״̬�Ƿ�Ϊһ������״̬
	PL0TokenType target_token_type;                      // �������һ������״̬����ʶĿ��ʷ���Ԫ
	int validator_number;                                // ��֤������
	BOOL(**validators)(char input, char param);          // ��֤�����飬����Ԫ�����ͣ�����ָ��
	char* params;                                        // �����֤����Ҫһ�������������ô����������ڵĶ�ӦԪ�ش���ȥ
	int* target_states;                                  // ����֤���������׵�Ŀ��״̬����
} DFAState;

typedef struct _PL0DFA
{
	BOOL is_avaliable;                                   // ָʾ�� DFA ��ǰ�Ƿ���Ч
	BOOL has_found_token;                                // ָʾ�� DFA �Ƿ��Ѿ��ҵ�һ�� TOKEN
	PL0TokenType token_found;                            // ��ʶ�Ѿ��ҵ��� TOKEN
	int current_state;
	int state_number;
	DFAState* states;                                    // ״̬���飬����״̬��ӵ�е�����ֵ��Ϊ���� ID
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

// ����Ƿ��Ѿ��ҵ��� TOKEN
void DFAList_Validate(DFAList* list, BOOL* has_found_token, PL0TokenType* token_found);

void DFA_Reset(DFA* dfa);

// �������� DFA ��״̬
void DFAList_Reset(DFAList* list);

void DFA_Input(DFA* dfa, char c);

#endif // !DFA_H
