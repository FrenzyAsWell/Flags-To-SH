#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARG_INPUT_COMMAND 2

int iArrSize = 256;
char buffer[256];
char updated_buffer[256];

void PrintData(char** arrErasable);
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

	PrintData(arrOptions);
	EraseData(arrOptions);

	return 0;
}

char** GetOptions(char* pCommand)
{	
	char *pNewCommand;
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

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '=' || buffer[b] == ' ')
				break;
			updated_buffer[b] = buffer[b];
		}

		pNewCommand = strtok(updated_buffer, "");

		arrOptions[a] = malloc((strlen(pNewCommand) + 1) * sizeof(char));
		strcpy(arrOptions[a], pNewCommand);
	}
	return arrOptions;
}

void PrintData(char** arrErasable)
{
	for (int a = 0; a < iArrSize; a++)
		if (arrErasable[a] != NULL) 
			printf("%s\n", arrErasable[a]);
}

void EraseData(char** arrErasable)
{
	for (int a = 0; a < iArrSize; a++)
		if (arrErasable[a] != NULL) 
			free(arrErasable[a]);
	free(arrErasable);
}
