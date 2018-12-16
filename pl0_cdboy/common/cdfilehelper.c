#include<stdio.h>
#include<stdlib.h>
#include"cdfilehelper.h"

void cdfh_GetOpenedFile(Cdfh* fh, FILE* file, int current_row, int current_column)
{
	fh->file = file;
	fh->current_column = current_column;
	fh->current_row = current_row;
}

errno_t cdfh_OpenRead(const char* filename, Cdfh* fh)
{
	fh->current_column = 0;
	fh->current_row = 1;
	return fopen_s(&(fh->file), filename, "r");
}

void cdfh_Close(Cdfh* fh)
{
	fclose(fh->file);
	fh->file = 0;
}

int cdfh_GetC(Cdfh* fh)
{
	FILE* file = fh->file;
	if (feof(file))
	{
		return EOF;
	}

	int c = fgetc(file);
	if (EOF != c)
	{
		fh->last_column = fh->current_column;
		if ('\n' == c)
		{
			fh->current_row++;
			fh->current_column = 0;
		}
		else
		{
			fh->current_column++;
		}
	}

	return c;
}

int cdfh_SeekMinus1(Cdfh* fh)
{
	if (0 == fh->current_column)
	{
		fh->current_row = fh->current_row > 1 ? fh->current_row - 1 : 1;
	}
	fh->current_column = fh->last_column;

	return fseek(fh->file, -1, SEEK_CUR);
}

int cdfh_IsWhiteSpace(char c, char ws[], int ws_num)
{
	for (int i = 0; i < ws_num; i++)
	{
		if (c == ws[i])
		{
			return 1;
		}
	}

	return 0;
}

int cdfh_ReadToSpace(Cdfh* fh, char* str, int maxread)
{
	FILE* file = fh->file;
	if (feof(file))
	{
		return 0;
	}
	if (maxread <= 0)
	{
		return 0;
	}

	char ws[] = { ' ', '\r', '\n', '\t' };
	int ws_num = 4;

	int c = '\0';
	int read_count = 0;

	// 读到没有空白为止
	while (!feof(file))
	{
		c = cdfh_GetC(fh);
		if (cdfh_IsWhiteSpace((char)c, ws, ws_num))
		{
			continue;
		}
		else
		{
			break;
		}
	}

	if (cdfh_IsWhiteSpace((char)c, ws, ws_num))
	{
		return 0;
	}
	else
	{
		str[0] = (char)c;
		read_count++;

		while (!feof(file) && read_count < maxread)
		{
			c = cdfh_GetC(fh);
			if (cdfh_IsWhiteSpace((char)c, ws, ws_num))
			{
				break;
			}
			else
			{
				str[read_count] = (char)c;
				read_count++;
			}
		}

		str[read_count + 1] = '\0';
		return read_count;
	}
}

int cdfh_ReadInt(Cdfh* fh, int maxread, int* result)
{
	char* str = (char*)calloc(maxread + 2, sizeof(char));  // 本来这里是加 1 的，但会出错，下面那个 3 同理

	int read_count = cdfh_ReadToSpace(fh, str, maxread);

	if (0 == read_count)
	{
		return 0;
	}

	*result = atoi(str);
	
	free(str);
	return read_count;
}

int cdfh_ReadChar(Cdfh* fh, char* result)
{
	char* str = (char*)calloc(3, sizeof(char));

	int read_count = cdfh_ReadToSpace(fh, str, 1);

	if (0 == read_count)
	{
		return 0;
	}

	*result = str[0];

	free(str);
	return read_count;
}