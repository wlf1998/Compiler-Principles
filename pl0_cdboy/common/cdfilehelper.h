#pragma once
#ifndef CDFH_H
#define CDFH_H

typedef struct CDFileHelper
{
	FILE* file;
	int current_row;
	int current_column;
	int last_column;
} Cdfh;

void cdfh_GetOpenedFile(Cdfh* fh, FILE* file, int current_row, int current_column);

errno_t cdfh_OpenRead(const char* filename, Cdfh* fh);

void cdfh_Close(Cdfh* fh);

int cdfh_GetC(Cdfh* fh);

int cdfh_SeekMinus1(Cdfh* fh);

int cdfh_IsWhiteSpace(char c, char ws[], int ws_num);

// ����д�� str ���ַ�����
int cdfh_ReadToSpace(Cdfh* fh, char* str, int maxread);

// ����д�� str ���ַ�����
int cdfh_ReadInt(Cdfh* fh, int maxread, int* result);

// ����д�� str ���ַ�����
int cdfh_ReadChar(Cdfh* fh, char* result);

#endif
