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

char buffer[256];
char updated_buffer[256];

void InteractData(WINDOW* wndMain, char** arrErasable, int iSize);
int GetOptions(char* pCommand, char*** arrOptions);
void EraseData(char** arrErasable, int iSize);
void PrintData(WINDOW* wndMain, stWindowSize* twndSize, char** arrPrintable, int iSize, int iHighlighted);


int main(int argc, char *argv[])
{
	if (argc != ARG_INPUT_COMMAND) 
	{
		printf("Help option");
		return 1;	
	}
	
	char** arrOptions;
	int iArrSize = GetOptions(argv[argc - 1], &arrOptions);

	initscr();
	curs_set(0);
	noecho();

	int ixMax, iyMax;
	getmaxyx(stdscr, iyMax, ixMax);

	WINDOW *wndOptions = newwin(iyMax / 2, ixMax / 2, 0, 0);
	keypad(wndOptions, true);

	InteractData(wndOptions, arrOptions, iArrSize);
	EraseData(arrOptions, iArrSize);

	endwin();

	return 0;
}

int GetOptions(char* pCommand, char*** arrOptions)
{	
	char *pNewCommand;
	FILE *fpHelp;

	strcat(pCommand, " --help");

	fpHelp = popen(pCommand, "r");

	int iLines = 0;
	for (int a = 0; fgets(buffer, sizeof(buffer), fpHelp) != NULL; a++)
		iLines++;

	(*arrOptions) = malloc(iLines * sizeof(char*));

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

		(*arrOptions)[a] = malloc((strlen(pNewCommand) + 1) * sizeof(char*));
		strcpy((*arrOptions)[a], pNewCommand);
	}

	fclose(fpHelp);
}

void PrintData(WINDOW* wndMain, stWindowSize* twndSize, char** arrPrintable, int iSize, int iHighlighted)
{ 
	int ixWin = 0;
	int iOffset = 1;

	refresh();
	for (int a = 0; a < iSize; a++)
		if (arrPrintable[a] != NULL) 
		{
			if (a == iHighlighted)
				wattron(wndMain, A_REVERSE);

			mvwprintw(wndMain, iOffset, 1, "%s\n", arrPrintable[a]);
			refresh();

			if (ixWin < strlen(arrPrintable[a])) 
				ixWin = strlen(arrPrintable[a]);

			wattroff(wndMain, A_REVERSE);
			iOffset++;
		}



	box(wndMain, 0, 0);
	wrefresh(wndMain);

	twndSize->x_size = ixWin;
	twndSize->y_size = iOffset;
}

void InteractData(WINDOW* wndMain, char** arrPrintable, int iSize)
{
	int iwndxOffset = 10; // HARDCODE HERE
	stWindowSize twndSize;

	PrintData(wndMain, &twndSize, arrPrintable, iSize, -1);
	wresize(wndMain, twndSize.y_size, twndSize.x_size + iwndxOffset + 2);

	int iInterface = 0;
	int iHighlighted = 0;
	while (1) 
	{
		PrintData(wndMain, &twndSize, arrPrintable, twndSize.y_size, iHighlighted);

		iInterface = wgetch(wndMain);
		switch (iInterface) 
		{
			case KEY_UP: 
			{			
				iHighlighted--;			
				if (iHighlighted < 3)
					iHighlighted = 3;	

				break;
			};
			case KEY_DOWN:
			{
				iHighlighted++;
				if (iHighlighted > twndSize.y_size)
					iHighlighted = twndSize.y_size;	

				break;
			}
			default: break;
		}

	}

	getch();
}

void EraseData(char** arrErasable, int iSize)
{
	for (int a = 0; a < iSize; a++)
		if (arrErasable[a] != NULL) 
			free(arrErasable[a]);
	free(arrErasable);
}
