#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARG_INPUT_COMMAND 2

int iArrSize = 256;
char buffer[256];

char** GetOptions(char* pCommand);
void EraseData(char** arrErasable);

int main(int argc, char *argv[])
{
	if (argc != ARG_INPUT_COMMAND) 
	{
		printf("Help option");
		return 1;	
	}
	
	char** arrOptions = GetOptions(argv[argc - 1]);

	EraseData(arrOptions);

	return 0;
}

char** GetOptions(char* pCommand)
{	
	char pNewCommand[256];
	FILE *fpHelp;

	char** arrOptions = malloc(iArrSize * sizeof(char*));

	strcat(pCommand, " --help");

	fpHelp = popen(pCommand, "r");
	for (int a = 0; fgets(buffer, sizeof(buffer), fpHelp) != NULL; a++)
	{
		char *pNew = strstr(buffer, "--");
		if (pNew == NULL) 
			continue;
			
		memmove(buffer, pNew + 2, strlen(pNew + 1) + 1);
		for (int b = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '=' || buffer[b] == ' ')
				break;
			pNewCommand[b] = buffer[b];
		}

		arrOptions[a] = malloc(strlen(pNewCommand) * sizeof(char));
		strcpy(arrOptions[a], pNewCommand);
	}

	return arrOptions;
}

void EraseData(char** arrErasable)
{
	for (int a = 0; a < iArrSize; a++)
		if (arrErasable[a] != NULL) 
			free(arrErasable[a]);
	free(arrErasable);
}
