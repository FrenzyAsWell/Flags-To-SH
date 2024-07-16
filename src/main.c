#include <stdio.h>
#include <string.h>

#define ARG_INPUT_COMMAND 2

char buffer[512];

int main(int argc, char *argv[])
{
	char *pCommand;
	FILE *fpHelp;
	char *pNewCommand;

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
		if (strstr(buffer, "--") == NULL) 
			continue;
		/*
		pNewCommand = NULL;
		for (int a = 0; a < strlen(buffer); a++) 
		{
			if (buffer[a] == '=')
				break;
			pNewCommand += buffer[a];
		}
		*/
		printf("%s", buffer);
	}

	return 0;
}
