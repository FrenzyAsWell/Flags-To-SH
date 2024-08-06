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
	char* data;
	char* param;
} stDataField;

typedef struct {
	int* list_selected;
	int count_selected;
} stSelection;

typedef struct {
	WINDOW* hWindow;
	int window_id;
	int offset_y;
	int offset_x;

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
	stDataField* dataFlags;
	stDisplayWindow wndDescription;
	stDataField* dataDesciption;

	stDisplayWindow wndResult;

	stSelection object_selection;
	int count_options;
	char* executable_name;
} stMainWindow;

char buffer[256];
char updated_buffer[256];

typedef enum 
{
	WND_FLAGS = 1,
	WND_RESULT = 2,
	WND_DESCRIPTION = 3,
	WND_DIALOG = 4,
	WND_ERROR = 5
} WND_CODES;

void InteractData(stMainWindow arrErasable);
void GetOptions(stMainWindow* arrOptions, char* pCommand);
void EraseStruct(stMainWindow arrErasable);
void PrintWindowData(char* sExecName, stSelection objSelection, stDisplayWindow wndSub, stDataField* _data,  int iSize, int iHighlighted, int* iyOffset, int* ixOffset);
void AddSelection(stSelection* object_selection, int iHighlighted);
void GetFitSize(stDataField* arrData, int iSize, stWindowSize* twndSize);
char* DisplayMessage(int _id, char* sMessage);
void SetParameter(stDataField* wndFlags, int iHighlighted);
int WriteSH(stSelection objSelection, char* sExecName, stDataField* wndFlags);

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
		char *pNew = strstr(buffer, "--"); if (pNew != NULL) 
			iLines++;
	}

	arrOptions->dataFlags = malloc(iLines * sizeof(stDataField));
	arrOptions->dataDesciption = malloc(iLines * sizeof(stDataField));

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

		arrOptions->dataFlags[a].data = malloc((strlen(pNewFlag) + 1) * sizeof(char*));
		strcpy(arrOptions->dataFlags[a].data, pNewFlag);

		memset(updated_buffer, '\0', sizeof(updated_buffer));
		for (int b = iPosSymbol + 1, c = 0; b < strlen(buffer) + 1; b++) 
		{
			if (buffer[b] == '\n' || (buffer[b - 1] == ' ' && buffer[b] == ' '))
				continue;

			updated_buffer[c] = buffer[b];
			c++;
		}
		pNewDescription = strtok(updated_buffer, "");

		arrOptions->dataDesciption[a].data = malloc((strlen(pNewDescription) + 1) * sizeof(char*));
		strcpy(arrOptions->dataDesciption[a].data, pNewDescription);
	}

	arrOptions->count_options = iLines;
	fclose(fpHelp);
}

void GetFitSize(stDataField* arrData, int iSize, stWindowSize* twndSize) {
	int ixWin = 0;

	for (int a = 0; a < iSize; a++)
		if (ixWin < strlen(arrData[a].data))
			ixWin = strlen(arrData[a].data);
	
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

void PrintWindowData(char* sExecName, stSelection objSelection, stDisplayWindow wndSub, stDataField* _data,  int iSize, int iHighlighted, int* iyOffset, int* ixOffset)
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
				for (int c = 0; c < objSelection.count_selected; c++) 
					if (objSelection.list_selected[c] == b) 
					{
						init_pair(1, COLOR_GREEN, 0);
						wattron(wndSub.hWindow, COLOR_PAIR(1));

						break;
					}

				if (b == iHighlighted)
					wattron(wndSub.hWindow, A_REVERSE);

				mvwprintw(wndSub.hWindow, a + 1, 1, "%s\n", _data[b].data);
				refresh();

				wattroff(wndSub.hWindow, A_REVERSE);
				wattroff(wndSub.hWindow, COLOR_PAIR(1));
			}
		}
		break;
		case WND_RESULT:
			{
				werase(wndSub.hWindow);

				mvwprintw(wndSub.hWindow, 1, 1, "%s", sExecName);
				*ixOffset += (strlen(sExecName) + 1);

				for (int a = 0; a < objSelection.count_selected; a++) 
				{
					if (_data[a].param != NULL) 
					{
						mvwprintw(wndSub.hWindow, 1, *ixOffset + 1, "--%s=%s",  _data[objSelection.list_selected[a]].data, _data[objSelection.list_selected[a]].param);
						*ixOffset += (strlen(_data[objSelection.list_selected[a]].data + strlen(_data[objSelection.list_selected[a]].param)) + 3);
					}
					else
					{
						mvwprintw(wndSub.hWindow, 1, *ixOffset + 1, "--%s", _data[objSelection.list_selected[a]].data);
						*ixOffset += (strlen(_data[objSelection.list_selected[a]].data) + 3);
					}
				}
				*ixOffset = 0;
			}
		break;
		case WND_DESCRIPTION:
			if (_data[iHighlighted].data != NULL)
			{
				werase(wndSub.hWindow);
				mvwprintw(wndSub.hWindow, 1, 1, "%s", _data[iHighlighted].data);
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

	GetFitSize(arrPrintable.dataFlags, arrPrintable.count_options, &twndSize);

	arrPrintable.wndFlags.hWindow = newwin(getmaxy(stdscr), twndSize.x_size + 10, 0, 0);
	arrPrintable.wndFlags.window_name = "Flags";
	arrPrintable.wndFlags.window_id = WND_FLAGS;

	arrPrintable.wndResult.hWindow = newwin(3, getmaxx(stdscr) - getmaxx(arrPrintable.wndFlags.hWindow), getmaxy(stdscr) - 3, getmaxx(arrPrintable.wndFlags.hWindow));
	arrPrintable.wndResult.window_name = "Result";
	arrPrintable.wndResult.window_id = WND_RESULT;

	arrPrintable.wndDescription.hWindow = newwin(getmaxy(stdscr) - getmaxy(arrPrintable.wndResult.hWindow), getmaxx(stdscr) - getmaxx(arrPrintable.wndFlags.hWindow), 0, getmaxx(arrPrintable.wndFlags.hWindow));
	arrPrintable.wndDescription.window_name = "Description";
	arrPrintable.wndDescription.window_id = WND_DESCRIPTION;

	keypad(arrPrintable.wndFlags.hWindow, true);

	arrPrintable.object_selection.count_selected = 0;
	arrPrintable.object_selection.list_selected = malloc(arrPrintable.object_selection.count_selected * sizeof(int));

	arrPrintable.wndFlags.offset_y = 0;
	arrPrintable.wndResult.offset_x = 0;
	while (1) 
	{
		PrintWindowData(arrPrintable.executable_name, arrPrintable.object_selection, arrPrintable.wndFlags, arrPrintable.dataFlags, arrPrintable.count_options, iHighlighted, &arrPrintable.wndFlags.offset_y, 0);
		PrintWindowData(arrPrintable.executable_name, arrPrintable.object_selection, arrPrintable.wndDescription, arrPrintable.dataDesciption, 1, iHighlighted, &arrPrintable.wndFlags.offset_y, 0);
		PrintWindowData(arrPrintable.executable_name, arrPrintable.object_selection, arrPrintable.wndResult, arrPrintable.dataFlags,  1, -1, &arrPrintable.wndResult.offset_y, &arrPrintable.wndResult.offset_x);

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
			case 121: WriteSH(arrPrintable.object_selection, arrPrintable.executable_name, arrPrintable.dataFlags); break;
			case 101: SetParameter(arrPrintable.dataFlags, iHighlighted); break;
		}

	}

	getch();
}

void SetParameter(stDataField* wndFlags, int iHighlighted)
{
	char* sResponse = DisplayMessage(WND_DIALOG, "Add value: ");

	wndFlags[iHighlighted].param = sResponse;
}

char* DisplayMessage(int _id, char* sMessage)
{
	char sResponse[64];

	stSystemWindow wndMessage;
	wndMessage.hWindow = newwin(3, getmaxx(stdscr) / 2, getmaxy(stdscr) / 2, getmaxx(stdscr) / 4);

	switch (_id)
	{
		case WND_DIALOG:
			{
				init_pair(2, COLOR_YELLOW, 0);
				wattron(wndMessage.hWindow, 2);

				wndMessage.window_name = "Dialog";
			};
		break;
		case WND_ERROR:
			{
				init_pair(3, COLOR_RED, 0);
				wattron(wndMessage.hWindow, 3);

				wndMessage.window_name = "Error";
			};
		break;
	}

	box(wndMessage.hWindow, 0, 0);
	mvwprintw(wndMessage.hWindow, 0, 2, " %s ", wndMessage.window_name);

	wattroff(wndMessage.hWindow, 2);
	wattroff(wndMessage.hWindow, 3);

	mvwprintw(wndMessage.hWindow, 1, 1, " %s", sMessage);
	wrefresh(wndMessage.hWindow);

	mvwgetstr(wndMessage.hWindow, 1, strlen(sMessage) + 1, sResponse);

	werase(wndMessage.hWindow);

	char* sReturn = malloc(strlen(sResponse) * sizeof(char));
	strcpy(sReturn, sResponse);
	
	return sReturn;
}

int WriteSH(stSelection objSelection, char* sExecName, stDataField* wndFlags)
{
	if (objSelection.count_selected < 1)
		return 1;

	FILE* fpFile;

	char* sPrevigies = NULL;
	char* sResponse = DisplayMessage(WND_DIALOG, "Enter name of SH: ");

	char* pCommand = malloc(strlen(sExecName) * sizeof(char));
	strcat(pCommand, sExecName);
	for (int a = 0; a < objSelection.count_selected; a++)
	{
		strcat(pCommand, " --");
		pCommand = realloc(pCommand, strlen(pCommand) + strlen(wndFlags[objSelection.list_selected[a]].data) * sizeof(char));
		strcat(pCommand, wndFlags[objSelection.list_selected[a]].data);
	}			

	fpFile = fopen(sResponse, "r");
	if (fpFile != NULL)
	{
		char* sAnswer = DisplayMessage(WND_ERROR, "Do you want to rewrite file? [y]: ");
		if (strcmp(sAnswer, "y") > 0)
			return 1;

		fclose(fpFile);
	}

	fpFile = fopen(sResponse, "wb");
	if (fpFile == NULL)
	{
		DisplayMessage(WND_ERROR, "Can't create window");
		return 1;
	}	

	fputs(pCommand, fpFile);
	chmod(sResponse, S_IRUSR | S_IWUSR | S_IXUSR);

	fclose(fpFile);
	free(pCommand);

	return 0;
}

void EraseStruct(stMainWindow arrErasable)
{
	for (int a = 0; a < arrErasable.count_options; a++)
	{
		free(arrErasable.dataFlags[a].data);
		free(arrErasable.dataFlags[a].param);
		free(arrErasable.dataDesciption[a].data);
		free(arrErasable.dataDesciption[a].param);
	}

	free(arrErasable.dataFlags->data);
	free(arrErasable.dataFlags->param);
	free(arrErasable.dataDesciption->data);
	free(arrErasable.dataDesciption->param);
}
