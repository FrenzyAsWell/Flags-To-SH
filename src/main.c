#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ncurses.h>
#include <sys/stat.h>

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
	int window_id;
	int offset_y;
	int offset_x;

	char** window_data;
	const char* window_name;
} stDisplayWindow;

typedef struct {
	WINDOW* hWindow;
	int window_id;

	char* window_data;
	const char* window_name;
} stSystemWindow;

typedef struct {
	stDisplayWindow wndFlags;
	stDisplayWindow wndDescription;
	stDisplayWindow wndResult;
	stSystemWindow wndDialog;
	stSystemWindow wndError;

	stSelection object_selection;
	int count_options;
	char* executable_name;
} stMainWindow;

char buffer[256];
char updated_buffer[256];

enum 
{
	WND_FLAGS = 1,
	WND_RESULT = 2,
	WND_DESCRIPTION = 3,
	WND_DIALOG = 4,
	WND_ERROR = 5
};

void InteractData(stMainWindow arrErasable);
void GetOptions(stMainWindow* arrOptions, char* pCommand);
void EraseStruct(stMainWindow arrErasable);
void PrintWindowData(stMainWindow wndMain, stDisplayWindow wndSub, int iSize, int iHighlighted, int* iyOffset, int* ixOffset);
void AddSelection(stSelection* object_selection, int iHighlighted);
void GetFitSize(char** arrData, int iSize, stWindowSize* twndSize);
void DisplayMessage(stSystemWindow wndMessage, char* sMessage);
int WriteSH(stMainWindow wndMain);

int main(int argc, char *argv[])
{
	if (argc != ARG_INPUT_COMMAND) 
	{
		printf("Help option");
		return 1;	
	}

	stMainWindow arrData;
	arrData.executable_name = malloc(strlen(argv[argc - 1]) * sizeof(argv[argc - 1]));
	strcpy(arrData.executable_name, argv[argc - 1]);

	GetOptions(&arrData, argv[argc - 1]);

	initscr();
	start_color();
	curs_set(0);

	InteractData(arrData);
	EraseStruct(arrData);

	free(arrData.executable_name);

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

	arrOptions->wndFlags.window_data = malloc(iLines * sizeof(char*));
	arrOptions->wndDescription.window_data = malloc(iLines * sizeof(char*));

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

		arrOptions->wndFlags.window_data[a] = malloc((strlen(pNewFlag) + 1) * sizeof(char*));
		strcpy(arrOptions->wndFlags.window_data[a], pNewFlag);

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = iPosSymbol + 1, c = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '\n' || (buffer[b - 1] == ' ' && buffer[b] == ' '))
				continue;

			updated_buffer[c] = buffer[b];
			c++;
		}
		pNewDescription = strtok(updated_buffer, "");

		arrOptions->wndDescription.window_data[a] = malloc((strlen(pNewDescription) + 1) * sizeof(char*));
		strcpy(arrOptions->wndDescription.window_data[a], pNewDescription);
	}

	arrOptions->count_options = iLines;
	fclose(fpHelp);
}

void GetFitSize(char** arrData, int iSize, stWindowSize* twndSize) {
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

void PrintWindowData(stMainWindow wndMain, stDisplayWindow wndSub, int iSize, int iHighlighted, int* iyOffset, int* ixOffset)
{ 
	switch (wndSub.window_id) 
	{
		case WND_FLAGS:
		{
			int yMax = getmaxy(wndSub.hWindow);
			if (iHighlighted > getmaxy(wndSub.hWindow) - 3)
				*iyOffset = iHighlighted - (getmaxy(wndSub.hWindow) - 3);

			for (int a = 0, b = *iyOffset; b < iSize; a++, b++)
			{
				for (int c = 0; c < wndMain.object_selection.count_selected; c++) 
					if (wndMain.object_selection.list_selected[c] == b) 
					{
						init_pair(1, COLOR_GREEN, 0);
						wattron(wndSub.hWindow, COLOR_PAIR(1));

						break;
					}

				if (b == iHighlighted)
					wattron(wndSub.hWindow, A_REVERSE);

				mvwprintw(wndSub.hWindow, a + 1, 1, "%s\n", wndSub.window_data[b]);
				refresh();

				wattroff(wndSub.hWindow, A_REVERSE);
				wattroff(wndSub.hWindow, COLOR_PAIR(1));
			}
		}
		break;
		case WND_RESULT:
			{
				werase(wndSub.hWindow);

				mvwprintw(wndSub.hWindow, 1, 1, "%s", wndMain.executable_name);
				*ixOffset += (strlen(wndMain.executable_name) + 1);

				for (int a = 0; a < wndMain.object_selection.count_selected; a++) 
				{
					mvwprintw(wndSub.hWindow, 1, *ixOffset + 1, "--%s",  wndMain.wndFlags.window_data[wndMain.object_selection.list_selected[a]]);
					*ixOffset += (strlen(wndMain.wndFlags.window_data[wndMain.object_selection.list_selected[a]]) + 3);
				}
				*ixOffset = 0;
			}
		break;
		case WND_DESCRIPTION:
			if (wndSub.window_data != NULL)
			{
				werase(wndSub.hWindow);
				mvwprintw(wndSub.hWindow, 1, 1, "%s", wndSub.window_data[iHighlighted]);
			}
		break;
	}

	box(wndSub.hWindow, 0, 0);

	mvwprintw(wndSub.hWindow, 0, 2, " %s ", wndSub.window_name);
	wrefresh(wndSub.hWindow);
}

void InteractData(stMainWindow arrPrintable)
{
	int iInterface = 0;
	int iHighlighted = 0;

	int ixMax, iyMax;
	getmaxyx(stdscr, iyMax, ixMax);
	stWindowSize twndSize;

	GetFitSize(arrPrintable.wndFlags.window_data, arrPrintable.count_options, &twndSize);

	arrPrintable.wndFlags.hWindow = newwin(getmaxy(stdscr), twndSize.x_size + 10, 0, 0);
	arrPrintable.wndFlags.window_name = "Flags";
	arrPrintable.wndFlags.window_id = WND_FLAGS;

	arrPrintable.wndResult.hWindow = newwin(3, getmaxx(stdscr) - getmaxx(arrPrintable.wndFlags.hWindow), getmaxy(stdscr) - 3, getmaxx(arrPrintable.wndFlags.hWindow));
	arrPrintable.wndResult.window_name = "Result";
	arrPrintable.wndResult.window_id = WND_RESULT;

	arrPrintable.wndDescription.hWindow = newwin(getmaxy(stdscr) - getmaxy(arrPrintable.wndResult.hWindow), getmaxx(stdscr) - getmaxx(arrPrintable.wndFlags.hWindow), 0, getmaxx(arrPrintable.wndFlags.hWindow));
	arrPrintable.wndDescription.window_name = "Description";
	arrPrintable.wndDescription.window_id = WND_DESCRIPTION;

	arrPrintable.wndDialog.hWindow = newwin(3, getmaxx(stdscr) / 2, getmaxy(stdscr) / 2, getmaxx(stdscr) / 4);
	arrPrintable.wndDialog.window_name = "Dialog";
	arrPrintable.wndDialog.window_id = WND_DIALOG;

	arrPrintable.wndError.hWindow = newwin(3, getmaxx(stdscr) / 2, getmaxy(stdscr) / 2, getmaxx(stdscr) / 4);
	arrPrintable.wndError.window_name = "Error";
	arrPrintable.wndError.window_id = WND_ERROR;

	keypad(arrPrintable.wndFlags.hWindow, true);

	arrPrintable.object_selection.count_selected = 0;
	arrPrintable.object_selection.list_selected = malloc(arrPrintable.object_selection.count_selected * sizeof(int));

	arrPrintable.wndFlags.offset_y = 0;
	arrPrintable.wndResult.offset_x = 0;
	while (1) 
	{
		PrintWindowData(arrPrintable, arrPrintable.wndFlags, arrPrintable.count_options, iHighlighted, &arrPrintable.wndFlags.offset_y, 0);
		PrintWindowData(arrPrintable, arrPrintable.wndDescription, 1, iHighlighted, &arrPrintable.wndFlags.offset_y, 0);
		PrintWindowData(arrPrintable, arrPrintable.wndResult, 1, -1, &arrPrintable.wndResult.offset_y, &arrPrintable.wndResult.offset_x);

		iInterface = wgetch(arrPrintable.wndFlags.hWindow);
		switch (iInterface) 
		{
			case 107:
			case KEY_UP: 
			{			
				iHighlighted--;			
				if (iHighlighted < arrPrintable.wndFlags.offset_y)
					iHighlighted = arrPrintable.wndFlags.offset_y;	
			};
			break;

			case 106:
			case KEY_DOWN:
			{
				iHighlighted++;

				if (iHighlighted > arrPrintable.count_options - 1)
					iHighlighted = arrPrintable.count_options - 1;
			};
			break;

			case 10: AddSelection(&arrPrintable.object_selection, iHighlighted); break;
			case 121: WriteSH(arrPrintable); break;
			case 101: break;
		}

	}

	getch();
}

void DisplayMessage(stSystemWindow wndMessage, char* sMessage)
{
	switch (wndMessage.window_id)
	{
		case WND_DIALOG:
			{
				init_pair(2, COLOR_YELLOW, 0);
				wattron(wndMessage.hWindow, 2);
			};
		break;
		case WND_ERROR:
			{
				init_pair(3, COLOR_RED, 0);
				wattron(wndMessage.hWindow, 3);
			};
		break;
	}

	box(wndMessage.hWindow, 0, 0);

	mvwprintw(wndMessage.hWindow, 0, 2, " %s ", wndMessage.window_name);
	mvwprintw(wndMessage.hWindow, 1, 1, " %s", sMessage);
	wrefresh(wndMessage.hWindow);

	wattroff(wndMessage.hWindow, 3);

}

int WriteSH(stMainWindow wndMain)
{
	if (wndMain.object_selection.count_selected < 1)
		return 1;

	FILE* fpFile;

	char* sMessage = "Enter name of SH: ";
	char* sErrorFileExist = "Do you want to rewrite file? [y]: ";
	char sResponse[32];
	char* sPrevigies = NULL;

	DisplayMessage(wndMain.wndDialog, sMessage);
	mvwgetnstr(wndMain.wndDialog.hWindow, 1, strlen(sMessage) + 1, sResponse, 32);

	werase(wndMain.wndDialog.hWindow);

	char* pCommand = malloc(strlen(wndMain.executable_name) * sizeof(char));
	strcat(pCommand, wndMain.executable_name);
	for (int a = 0; a < wndMain.object_selection.count_selected; a++)
	{
		strcat(pCommand, " --");
		pCommand = realloc(pCommand, strlen(pCommand) + strlen(wndMain.wndFlags.window_data[wndMain.object_selection.list_selected[a]]) * sizeof(char));
		strcat(pCommand, wndMain.wndFlags.window_data[wndMain.object_selection.list_selected[a]]);
	}			

	fpFile = fopen(sResponse, "r");
	if (fpFile != NULL)
	{
		DisplayMessage(wndMain.wndError, sErrorFileExist);
		char sAnswer = mvwgetch(wndMain.wndError.hWindow, 1, strlen(sErrorFileExist) + 1);

		if (sAnswer == 'y')
			return 1;

		fclose(fpFile);
	}

	fpFile = fopen(sResponse, "wb");
	if (fpFile == NULL)
	{
		DisplayMessage(wndMain.wndError, "Can't create window");
		return 1;
	}	

	fputs(pCommand, fpFile);
	chmod(sResponse, S_IRUSR | S_IWUSR | S_IXUSR);

	fclose(fpFile);
	free(pCommand);

	werase(wndMain.wndError.hWindow);

	return 0;
}

void EraseStruct(stMainWindow arrErasable)
{
	for (int a = 0; a < arrErasable.count_options; a++)
	{
		free(arrErasable.wndFlags.window_data[a]);
		free(arrErasable.wndDescription.window_data[a]);
	}

	free(arrErasable.wndFlags.window_data);
	free(arrErasable.wndDescription.window_data);
}
