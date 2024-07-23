#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARG_INPUT_COMMAND 2

typedef struct {
	char* name_flag;
	char* text_description;
} stFlags;

char buffer[256];
char updated_buffer[256];

void PrintData(stFlags* arrErasable, int iSize);
int GetOptions(char* pCommand, stFlags** arrOptions);
void EraseData(stFlags* arrErasable, int iSize);

int main(int argc, char *argv[])
{
	if (argc != ARG_INPUT_COMMAND) 
	{
		printf("Help option");
		return 1;	
	}
	
	stFlags* arrOptions;
	int iArrSize = GetOptions(argv[argc - 1], &arrOptions);

	PrintData(arrOptions, iArrSize);
	EraseData(arrOptions, iArrSize);

	return 0;
}

int GetOptions(char* pCommand, stFlags** arrOptions)
{	
	char *pNewCommand;
	FILE *fpHelp;

	strcat(pCommand, " --help");

	fpHelp = popen(pCommand, "r");

	int iLines = 0;
	for (int a = 0; fgets(buffer, sizeof(buffer), fpHelp) != NULL; a++)
	{
		char *pNew = strstr(buffer, "--");
		if (pNew != NULL) 
			iLines++;
	}

	(*arrOptions) = malloc(iLines * sizeof(char*));

	fpHelp = popen(pCommand, "r");
	for (int a = 0; fgets(buffer, sizeof(buffer), fpHelp) != NULL; a++)
	{
		char *pNew = strstr(buffer, "--");
		if (pNew == NULL) 
		{
			a--;
			continue;
		}

		memmove(buffer, pNew + 2, strlen(pNew + 1) + 1);

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '=' || buffer[b] == ' ')
				break;
			updated_buffer[b] = buffer[b];
		}

		pNewCommand = strtok(updated_buffer, "");

		(*arrOptions)[a].name_flag = malloc((strlen(pNewCommand) + 1) * sizeof(char*));
		strcpy((*arrOptions)[a].name_flag, pNewCommand);
	}

	fclose(fpHelp);

	return iLines;
}

void PrintData(stFlags* arrErasable, int iSize)
{
	for (int a = 0; a < iSize; a++)
		if (arrErasable[a].name_flag != NULL) 
			printf("%s\n", arrErasable[a].name_flag);
}

void EraseData(stFlags* arrErasable, int iSize)
{
	for (int a = 0; a < iSize; a++)
		if (arrErasable[a].name_flag != NULL) 
			free(arrErasable[a].name_flag);
	free(arrErasable);
}
