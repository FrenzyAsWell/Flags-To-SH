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
	WINDOW* hFlags;
	char** name_flag;

	int offset;
} stFlags;

typedef struct {
	WINDOW* hDescriptions;
	char** text_description;
} stDescription;

typedef struct {
	stFlags wndFlags;
	stDescription wndDescription;

	int count_options;
} stMainWindow;

char buffer[256];
char updated_buffer[256];

void InteractData(stMainWindow arrErasable);
void GetOptions(stMainWindow* arrOptions, char* pCommand);
void EraseStruct(stMainWindow arrErasable);
void PrintData(WINDOW* wndMain, char** arrPrintable, int iSize, int iHighlighted, int* iyOffset);
stWindowSize GetFitSize(char** arrData, int iSize);

int main(int argc, char *argv[])
{
	if (argc != ARG_INPUT_COMMAND) 
	{
		printf("Help option");
		return 1;	
	}
	
	stMainWindow arrData;
	GetOptions(&arrData, argv[argc - 1]);

	initscr();
	curs_set(0);
	noecho();

	InteractData(arrData);
	EraseStruct(arrData);

	endwin();

	return 0;
}

void GetOptions(stMainWindow* arrOptions, char* pCommand)
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

	(*arrOptions).wndFlags.name_flag = malloc(iLines * sizeof(char*));

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

		(*arrOptions).wndFlags.name_flag[a] = malloc((strlen(pNewFlag) + 1) * sizeof(char*));
		strcpy((*arrOptions).wndFlags.name_flag[a], pNewFlag);

		/*
		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = iPosSymbol, c = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '\n')
				continue;

			updated_buffer[c] = buffer[b];
			c++;
		}
		pNewDescription = strtok(updated_buffer, "");

		(*arrOptions).wndDescription.text_description[a] = malloc((strlen(pNewDescription) + 1) * sizeof(char*));
		strcpy((*arrOptions).wndDescription.text_description[a], pNewDescription);
		*/
	}

	arrOptions->count_options = iLines;
	//fclose(fpHelp);
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

void PrintData(WINDOW* wndMain, char** arrPrintable, int iSize, int iHighlighted, int* iyOffset)
{ 
	int yMax = getmaxy(wndMain);
	if (iHighlighted > getmaxy(wndMain) - 3)
		*iyOffset = iHighlighted - (getmaxy(wndMain) - 3);

	refresh();
	for (int a = 0, b = *iyOffset; b < iSize; a++, b++)
	{
		if (b == iHighlighted)
			wattron(wndMain, A_REVERSE);

		mvwprintw(wndMain, a + 1, 1, "%s\n", arrPrintable[b]);
		refresh();

		wattroff(wndMain, A_REVERSE);
	}

	box(wndMain, 0, 0);
	wrefresh(wndMain);
}

void InteractData(stMainWindow arrPrintable)
{
	int iInterface = 0;
	int iHighlighted = -1;

	int ixMax, iyMax;
	getmaxyx(stdscr, iyMax, ixMax);

	stWindowSize twndOptionSize = GetFitSize(arrPrintable.wndFlags.name_flag, arrPrintable.count_options);
	arrPrintable.wndFlags.hFlags = newwin(getmaxy(stdscr), twndOptionSize.x_size, 0, 0);
	
	keypad(arrPrintable.wndFlags.hFlags, true);

	arrPrintable.wndFlags.offset = 0;
	while (1) 
	{
		PrintData(arrPrintable.wndFlags.hFlags, arrPrintable.wndFlags.name_flag, arrPrintable.count_options, iHighlighted, &arrPrintable.wndFlags.offset);

		iInterface = wgetch(arrPrintable.wndFlags.hFlags);
		switch (iInterface) 
		{
			case KEY_UP: 
			{			
				iHighlighted--;			
				if (iHighlighted < arrPrintable.wndFlags.offset)
					iHighlighted = arrPrintable.wndFlags.offset;	

				break;
			};
			case KEY_DOWN:
			{
				iHighlighted++;

				if (iHighlighted > arrPrintable.count_options - 1)
					iHighlighted = arrPrintable.count_options - 1;

				break;
			}
			default: break;
		}

	}

	getch();
}

void EraseStruct(stMainWindow arrErasable)
{
	for (int a = 0; a < arrErasable.count_options; a++)
	{
		free(arrErasable.wndFlags.name_flag[a]);
		free(arrErasable.wndDescription.text_description[a]);
	}

	free(arrErasable.wndFlags.name_flag);
	free(arrErasable.wndDescription.text_description);
}
