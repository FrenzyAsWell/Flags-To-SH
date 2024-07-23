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

void InteractData(WINDOW* wndMain, stFlags* arrErasable, int iSize);
int GetOptions(char* pCommand, stFlags** arrOptions);
void EraseData(stFlags* arrErasable, int iSize);
void PrintData(WINDOW* wndMain, stFlags* arrPrintable, int iSize, int iHighlighted);

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

	int ixMax, iyMax;
	getmaxyx(stdscr, iyMax, ixMax);

	WINDOW *wndOptions = newwin(iyMax, ixMax, 0, 0);
	keypad(wndOptions, true);

	InteractData(wndOptions, arrOptions, iArrSize);
	EraseData(arrOptions, iArrSize);

	endwin();

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

void PrintData(WINDOW* wndMain, stFlags* arrPrintable, int iSize, int iHighlighted)
{ 
	int iwndxOffset = 10; // HARDCODE HERE
	int ixWin = 0, iyWin = 0;

	refresh();
	for (int a = 0; a < iSize; a++)
	{
		if (a == iHighlighted)
			wattron(wndMain, A_REVERSE);

		mvwprintw(wndMain, a + 1, 1, "%s\n", arrPrintable[a].name_flag);
		refresh();

		if (ixWin < strlen(arrPrintable[a].name_flag)) 
			ixWin = strlen(arrPrintable[a].name_flag);

		wattroff(wndMain, A_REVERSE);
	}

	iyWin = getmaxy(stdscr);
	wresize(wndMain, iyWin, ixWin + iwndxOffset + 2);

	box(wndMain, 0, 0);
	wrefresh(wndMain);
}

void InteractData(WINDOW* wndMain, stFlags* arrPrintable, int iSize)
{
	int iwndxOffset = 10; // HARDCODE HERE
	int iInterface = 0;
	int iHighlighted = -1;

	while (1) 
	{
		PrintData(wndMain, arrPrintable, iSize, iHighlighted);

		iInterface = wgetch(wndMain);
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

	getch();
}

void EraseData(stFlags* arrErasable, int iSize)
{
	for (int a = 0; a < iSize; a++)
		if (arrErasable[a].name_flag != NULL) 
			free(arrErasable[a].name_flag);
	free(arrErasable);
}
