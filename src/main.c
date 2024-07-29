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
	int* list_selected;
	int count_selected;
} stSelection;

typedef struct {
	WINDOW* hWindow;
	char** name_flag;

	stWindowSize object_size;
	stSelection object_selection;

	int offset;
	char* window_name;
} st_wndFlags;

typedef struct {
	WINDOW* hWindow;
	stWindowSize object_size;

	char** text_description;
	char* window_name;
} st_wndDescription;

typedef struct {
	WINDOW* hWindow;
	stWindowSize object_size;

	char** text_result;
	char* window_name;
} st_wndResult;

typedef struct {
	st_wndFlags wndFlags;
	st_wndDescription wndDescription;
	st_wndResult wndResult;

	int count_options;
} stMainWindow;

char buffer[256];
char updated_buffer[256];

void InteractData(stMainWindow arrErasable);
void GetOptions(stMainWindow* arrOptions, char* pCommand);
void EraseStruct(stMainWindow arrErasable);
void PrintWindowData(WINDOW* wndMain, char** arrPrintable, int iSize, int iHighlighted, stSelection stSelection, int* iyOffset, char* sWindowName);
void AddSelection(stSelection* object_selection, int iHighlighted);
void GetFitSize(char** arrData, int iSize, stWindowSize* twndSize);

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
	start_color();
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

	arrOptions->wndFlags.name_flag = malloc(iLines * sizeof(char*));
	arrOptions->wndDescription.text_description = malloc(iLines * sizeof(char*));

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

		arrOptions->wndFlags.name_flag[a] = malloc((strlen(pNewFlag) + 1) * sizeof(char*));
		strcpy(arrOptions->wndFlags.name_flag[a], pNewFlag);

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = iPosSymbol + 1, c = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '\n' || (buffer[b - 1] == ' ' && buffer[b] == ' '))
				continue;

			updated_buffer[c] = buffer[b];
			c++;
		}
		pNewDescription = strtok(updated_buffer, "");

		arrOptions->wndDescription.text_description[a] = malloc((strlen(pNewDescription) + 1) * sizeof(char*));
		strcpy(arrOptions->wndDescription.text_description[a], pNewDescription);
	}

	arrOptions->count_options = iLines;
	fclose(fpHelp);
}

void GetFitSize(char** arrData, int iSize, stWindowSize* twndSize) 
{
	int ixWin = 0;

	for (int a = 0; a < iSize; a++)
		if (ixWin < strlen(arrData[a]))
			ixWin = strlen(arrData[a]);
	
	twndSize->y_size = iSize;
	twndSize->x_size = ixWin;
}

void AddSelection(stSelection* object_selection, int iHighlighted)
{
	bool isSelected = false;
	int index = 0;

	for (int a = 0; a < object_selection->count_selected; a++)
		if (object_selection->list_selected[a] == iHighlighted) 
		{
			isSelected = true;
			index = a;

			object_selection->count_selected--;
			break;
		}

	if (isSelected == true) 
	{
		for (int b = index; b < object_selection->count_selected; b++)
        	object_selection->list_selected[b] = object_selection->list_selected[b + 1];
	}
	else
	{
		object_selection->list_selected = realloc(object_selection->list_selected, (object_selection->count_selected + 1) * sizeof(int));

		object_selection->list_selected[object_selection->count_selected] = iHighlighted;
		object_selection->count_selected++;
	}
}

void PrintWindowData(WINDOW* wndMain, char** arrPrintable, int iSize, int iHighlighted, stSelection stSelection, int* iyOffset, char* sWindowName)
{ 
	int yMax = getmaxy(wndMain);
	if (iHighlighted > getmaxy(wndMain) - 3)
		*iyOffset = iHighlighted - (getmaxy(wndMain) - 3);

	if (iSize > 1)
		for (int a = 0, b = *iyOffset; b < iSize; a++, b++)
		{
			for (int c = 0; c < stSelection.count_selected; c++) 
				if (stSelection.list_selected[c] == b) 
				{
					init_pair(1, COLOR_GREEN, 0);
					wattron(wndMain, COLOR_PAIR(1));

					break;
				}

			if (b == iHighlighted)
				wattron(wndMain, A_REVERSE);

			mvwprintw(wndMain, a + 1, 1, "%s\n", arrPrintable[b]);
			refresh();

			wattroff(wndMain, A_REVERSE);
			wattroff(wndMain, COLOR_PAIR(1));
		}
	else
		if (arrPrintable != NULL)
		{
			werase(wndMain);
			mvwprintw(wndMain, 1, 1, "%s", arrPrintable[iHighlighted]);
		}


	box(wndMain, 0, 0);

	mvwprintw(wndMain, 0, 2, " %s ", sWindowName);
	wrefresh(wndMain);
}

void InteractData(stMainWindow arrPrintable)
{
	int iInterface = 0;
	int iHighlighted = 0;

	int ixMax, iyMax;
	getmaxyx(stdscr, iyMax, ixMax);

	GetFitSize(arrPrintable.wndFlags.name_flag, arrPrintable.count_options, &arrPrintable.wndFlags.object_size);
	arrPrintable.wndFlags.object_size.x_size += 10;
	arrPrintable.wndFlags.hWindow = newwin(getmaxy(stdscr), arrPrintable.wndFlags.object_size.x_size, 0, 0);
	arrPrintable.wndFlags.window_name = "Flags";
	
	arrPrintable.wndResult.object_size.y_size = 3;
	arrPrintable.wndResult.object_size.x_size = getmaxx(stdscr) - arrPrintable.wndFlags.object_size.x_size;
	arrPrintable.wndResult.hWindow = newwin(arrPrintable.wndResult.object_size.y_size, 
											arrPrintable.wndResult.object_size.x_size, 
											getmaxy(stdscr) - arrPrintable.wndResult.object_size.y_size, 
											arrPrintable.wndFlags.object_size.x_size);
	arrPrintable.wndResult.window_name = "Result";
	
	arrPrintable.wndDescription.object_size.y_size = getmaxy(stdscr) - arrPrintable.wndResult.object_size.y_size;
	arrPrintable.wndDescription.object_size.x_size = getmaxx(stdscr) - arrPrintable.wndFlags.object_size.x_size;
	arrPrintable.wndDescription.hWindow = newwin(arrPrintable.wndDescription.object_size.y_size, 
														arrPrintable.wndDescription.object_size.x_size, 
														0, 
														arrPrintable.wndFlags.object_size.x_size);
	arrPrintable.wndDescription.window_name = "Description";

	keypad(arrPrintable.wndFlags.hWindow, true);

	arrPrintable.wndFlags.object_selection.count_selected = 0;
	arrPrintable.wndFlags.object_selection.list_selected = malloc(arrPrintable.wndFlags.object_selection.count_selected * sizeof(int));

	arrPrintable.wndFlags.offset = 0;
	while (1) 
	{
		PrintWindowData(arrPrintable.wndFlags.hWindow, 
						arrPrintable.wndFlags.name_flag, 
						arrPrintable.count_options, 
						iHighlighted, 
						arrPrintable.wndFlags.object_selection,
						&arrPrintable.wndFlags.offset, 
						arrPrintable.wndFlags.window_name);

		PrintWindowData(arrPrintable.wndDescription.hWindow, 
						arrPrintable.wndDescription.text_description, 
						1, 
						iHighlighted, 
						arrPrintable.wndFlags.object_selection,
						&arrPrintable.wndFlags.offset, 
						arrPrintable.wndDescription.window_name);

		PrintWindowData(arrPrintable.wndResult.hWindow,
						arrPrintable.wndResult.text_result,
						1,
						-1,
						arrPrintable.wndFlags.object_selection,
						&arrPrintable.wndFlags.offset,
						arrPrintable.wndResult.window_name);

		iInterface = wgetch(arrPrintable.wndFlags.hWindow);
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
			case 10:
			{
				AddSelection(&arrPrintable.wndFlags.object_selection, iHighlighted);

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
