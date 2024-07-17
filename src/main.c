#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARG_INPUT_COMMAND 2

char buffer[512];

int main(int argc, char *argv[])
{
	char *pCommand;
	char pNewCommand[256];
	FILE *fpHelp;

	if (argc != ARG_INPUT_COMMAND) 
	{
		printf("Help option");
		return 1;	
	}
	
	pCommand = argv[argc - 1];
	strcat(pCommand, " --help");

	fpHelp = popen(pCommand, "r");
	while (fgets(buffer, sizeof(buffer), fpHelp) != NULL)
	{
		char *pNew = strstr(buffer, "--");
		if (pNew == NULL) 
			continue;
			
		memmove(buffer, pNew + 1, strlen(pNew + 1) + 1);
		for (int a = 0; a < strlen(buffer) + 1; a++) 
		{
			if (buffer[a] == '=' || buffer[a] == ' ')
				break;
			pNewCommand[a] = buffer[a];
		}

		printf("%s\n", pNewCommand);
		memset(pNewCommand, '\0', sizeof(pNewCommand));
	}

	return 0;
}
