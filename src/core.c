#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "core.h"

char buffer[256];
char updated_buffer[256];

void GetOptions(stData* arrData, char* pCommand)
{	
	char *pNewFlag;
	char *pNewDescription;
	FILE *fpHelp;

	strcat(pCommand, " --help");

	fpHelp = popen(pCommand, "r");

	int iLines = 0;
	for (int a = 0; fgets(buffer, sizeof(buffer), fpHelp) != NULL; a++)
	{
		char *pNew = strstr(buffer, "--"); if (pNew != NULL) 
			iLines++;
	}

	arrData->dataFlags = malloc(iLines * sizeof(char));
	arrData->dataDescription = malloc(iLines * sizeof(char));

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

		int iPosSymbol = 0;

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = iPosSymbol; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '=' || buffer[b] == ' ')
				break;
			updated_buffer[b] = buffer[b];
			iPosSymbol = b;
		}

		pNewFlag = strtok(updated_buffer, "");
		strcpy(&arrData->dataFlags[a], pNewFlag);

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = iPosSymbol + 1, c = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '\n' || (buffer[b - 1] == ' ' && buffer[b] == ' '))
				continue;

			updated_buffer[c] = buffer[b];
			c++;
		}
		pNewDescription = strtok(updated_buffer, "");

		strcpy(&arrData->dataDescription[a], pNewDescription);
	}

	//arrOptions->count_options = iLines;
	fclose(fpHelp);
}

