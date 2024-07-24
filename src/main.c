#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#define ARG_INPUT_COMMAND 2

typedef struct 
{
	int x_size;
	int y_size;

} stWindowSize;

typedef struct {
	char* name_flag;
	char* text_description;
} stFlags;

char buffer[256];
char updated_buffer[256];

void InteractData(stFlags* arrErasable, int iSize);
int GetOptions(char* pCommand, stFlags** arrOptions);
void EraseStruct(stFlags* arrErasable, int iSize);
void EraseCharArray(char** arrErasable, int iSize);
void PrintData(WINDOW* wndMain, char** arrPrintable, int iSize, int iHighlighted);
stWindowSize GetFitSize(char** arrData, int iSize);

int main(int argc, char *argv[])
{
	if (argc != ARG_INPUT_COMMAND) 
	{
		printf("Help option");
		return 1;	
	}
	
	stFlags* arrOptions;
	int iArrSize = GetOptions(argv[argc - 1], &arrOptions);

	initscr();
	curs_set(0);
	noecho();

	InteractData(arrOptions, iArrSize);
	EraseStruct(arrOptions, iArrSize);

	endwin();

	return 0;
}

int GetOptions(char* pCommand, stFlags** arrOptions)
{	
	char *pNewFlag;
	char *pNewDescription;
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

		(*arrOptions)[a].name_flag = malloc((strlen(pNewFlag) + 1) * sizeof(char*));
		strcpy((*arrOptions)[a].name_flag, pNewFlag);

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = iPosSymbol, c = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '\n')
				continue;

			updated_buffer[c] = buffer[b];
			c++;
		}
		pNewDescription = strtok(updated_buffer, "");

		(*arrOptions)[a].text_description = malloc((strlen(pNewDescription) + 1) * sizeof(char*));
		strcpy((*arrOptions)[a].text_description, pNewDescription);

	}

	fclose(fpHelp);
	
	return iLines;
}

stWindowSize GetFitSize(char** arrData, int iSize) 
{
	stWindowSize twndSize;
	int ixWin = 0;

	for (int a = 0; a < iSize; a++)
		if (ixWin < strlen(arrData[a]))
			ixWin = strlen(arrData[a]);
	
	twndSize.y_size = iSize;
	twndSize.x_size = ixWin;
	
	return twndSize;
}

void PrintData(WINDOW* wndMain, char** arrPrintable, int iSize, int iHighlighted)
{ 
	int iwndxOffset = 10; // HARDCODE HERE

	refresh();
	for (int a = 0; a < iSize; a++)
	{
		if (a == iHighlighted)
			wattron(wndMain, A_REVERSE);

		mvwprintw(wndMain, a + 1, 1, "%s\n", arrPrintable[a]);
		refresh();

		wattroff(wndMain, A_REVERSE);
	}

	box(wndMain, 0, 0);
	wrefresh(wndMain);
}

void InteractData(stFlags* arrPrintable, int iSize)
{
	int iwndxOffset = 10; // HARDCODE HERE
	int iInterface = 0;
	int iHighlighted = -1;

	int ixMax, iyMax;
	getmaxyx(stdscr, iyMax, ixMax);

	char** arrDataOptions = (char**)malloc(iSize * sizeof(char*));
	for (int a = 0; a < iSize; a++) 
	{
		arrDataOptions[a] = (char*)malloc((strlen(arrPrintable[a].name_flag) + 1) * sizeof(char));
		strcpy(arrDataOptions[a], arrPrintable[a].name_flag);
	}

	stWindowSize twndOptionSize = GetFitSize(arrDataOptions, iSize);
	WINDOW *wndOptions = newwin(iyMax, twndOptionSize.x_size, 0, 0);
	
	char** arrDataDescription = (char**)malloc(iSize * sizeof(char*));
	for (int a = 0; a < iSize; a++) 
	{
		arrDataDescription[a] = (char*)malloc((strlen(arrPrintable[a].text_description) + 1) * sizeof(char));
		strcpy(arrDataDescription[a], arrPrintable[a].text_description);
	}

	stWindowSize twndDescriptionSize = GetFitSize(arrDataDescription, iSize);
	WINDOW *wndDesciptions = newwin(iyMax, ixMax - twndOptionSize.x_size, 0, twndOptionSize.x_size);

	keypad(wndOptions, true);

	while (1) 
	{
		PrintData(wndOptions, arrDataOptions, iSize, iHighlighted);
		PrintData(wndDesciptions, arrDataDescription, iSize, iHighlighted);

		iInterface = wgetch(wndOptions);
		switch (iInterface) 
		{
			case KEY_UP: 
			{			
				iHighlighted--;			
				if (iHighlighted < 0)
					iHighlighted = 0;	

				break;
			};
			case KEY_DOWN:
			{
				iHighlighted++;
				if (iHighlighted > iSize - 1)
					iHighlighted = iSize - 1;	

				break;
			}
			default: break;
		}

	}

	EraseCharArray(arrDataOptions, iSize);
	EraseCharArray(arrDataDescription, iSize);

	getch();
}

void EraseStruct(stFlags* arrErasable, int iSize)
{
	for (int a = 0; a < iSize; a++)
	{
		free(arrErasable[a].name_flag);
		free(arrErasable[a].text_description);
	}
	free(arrErasable);
}

void EraseCharArray(char** arrErasable, int iSize)
{
	for (int a = 0; a < iSize; a++)
		free(arrErasable[a]);
	free(arrErasable);
}
