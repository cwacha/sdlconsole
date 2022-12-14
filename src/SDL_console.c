/*
	SDL_console: An easy to use drop-down console based on the SDL library
	Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004 Clemens Wacha

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WHITOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library Generla Public
	License along with this library; if not, write to the Free
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Clemens Wacha
	reflex-2000@gmx.net
*/

/*  SDL_console.c
 *  Written By: Garrett Banuk <mongoose@mongeese.org>
 *  Code Cleanup and heavily extended by: Clemens Wacha <reflex-2000@gmx.net>
 */

#include "SDL_console.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "DT_drawtext.h"
#include "internal.h"
#include "utf8.h"

#ifdef HAVE_SDLIMAGE
#include "SDL_image.h"
#endif

/* This contains a pointer to the "topmost" console. The console that
 * is currently taking keyboard input. */
static ConsoleInformation *Topmost;

/*  Takes keys from the keyboard and inputs them to the console
	If the event was not handled (i.e. WM events or unknown ctrl-shift
	sequences) the function returns the event for further processing. */
SDL_Event *CON_Events(SDL_Event *event)
{
	if (Topmost == NULL)
		return event;
	if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.scancode == Topmost->ToggleKey)
		{
			if (CON_isOpen(Topmost))
			{
				CON_Hide(Topmost);
				SDL_StopTextInput();
			}
			else
			{
				CON_Show(Topmost);
				SDL_StartTextInput();
			}
			return NULL;
		}
	}
	if (!CON_isOpen(Topmost))
		return event;

	if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.mod & KMOD_CTRL)
		{
			/* CTRL pressed */
			switch (event->key.keysym.sym)
			{
			case SDLK_a:
				Cursor_Home(Topmost);
				break;
			case SDLK_e:
				Cursor_End(Topmost);
				break;
			case SDLK_c:
				Clear_Command(Topmost);
				break;
			case SDLK_l:
				Clear_History(Topmost);
				CON_UpdateConsole(Topmost);
				break;
			default:
				return event;
			}
		}
		else if (event->key.keysym.mod & KMOD_ALT)
		{
			/* the console does not handle ALT combinations! */
			return event;
		}
		else
		{
			switch (event->key.keysym.sym)
			{
			case SDLK_HOME:
				if (event->key.keysym.mod & KMOD_SHIFT)
				{
					Topmost->ConsoleScrollBack = Topmost->LineBuffer - 1;
					CON_UpdateConsole(Topmost);
				}
				else
				{
					Cursor_Home(Topmost);
				}
				break;
			case SDLK_END:
				if (event->key.keysym.mod & KMOD_SHIFT)
				{
					Topmost->ConsoleScrollBack = 0;
					CON_UpdateConsole(Topmost);
				}
				else
				{
					Cursor_End(Topmost);
				}
				break;
			case SDLK_PAGEUP:
				Topmost->ConsoleScrollBack += CON_LINE_SCROLL;
				if (Topmost->ConsoleScrollBack > Topmost->LineBuffer - 1)
					Topmost->ConsoleScrollBack = Topmost->LineBuffer - 1;

				CON_UpdateConsole(Topmost);
				break;
			case SDLK_PAGEDOWN:
				Topmost->ConsoleScrollBack -= CON_LINE_SCROLL;
				if (Topmost->ConsoleScrollBack < 0)
					Topmost->ConsoleScrollBack = 0;
				CON_UpdateConsole(Topmost);
				break;
			case SDLK_UP:
				Command_Up(Topmost);
				break;
			case SDLK_DOWN:
				Command_Down(Topmost);
				break;
			case SDLK_LEFT:
				Cursor_Left(Topmost);
				break;
			case SDLK_RIGHT:
				Cursor_Right(Topmost);
				break;
			case SDLK_BACKSPACE:
				Cursor_BSpace(Topmost);
				break;
			case SDLK_DELETE:
				Cursor_Del(Topmost);
				break;
			case SDLK_INSERT:
				Topmost->InsMode = 1 - Topmost->InsMode;
				break;
			case SDLK_TAB:
				CON_TabCompletion(Topmost);
				break;
			case SDLK_RETURN:
				if (strlen(Topmost->Command) > 0)
				{
					CON_AddHistoryCommand(Topmost, Topmost->Command);

					/* display the command including the prompt */
					CON_Out(Topmost, "%s%s", Topmost->Prompt, Topmost->Command);

					CON_Execute(Topmost, Topmost->Command);
					/* printf("Command: %s\n", Topmost->Command); */

					Clear_Command(Topmost);
					Topmost->CommandScrollBack = -1;
				}
				break;
			case SDLK_ESCAPE:
				/* deactivate Console */
				CON_Hide(Topmost);
				return NULL;
			default:
				break;
			}
		}
		return NULL;
	}
	if (event->type == SDL_TEXTINPUT)
	{
		if (Topmost->InsMode)
			Cursor_Add(Topmost, &(event->text));
		else
		{
			Cursor_Add(Topmost, &(event->text));
			Cursor_Del(Topmost);
		}
		// size_t strLen = SDL_strlen(event->text.text);
		// SDL_LogError(SDL_LOG_CATEGORY_ERROR, "character length: %lld code: 0x%x 0x%x", strLen, event->text.text[0] & 0xff, event->text.text[strLen - 1] & 0xff);
	}

	return event;
}

/* Updates the console, draws the background and the history lines. Does not draw the Commandline */
void CON_UpdateConsole(ConsoleInformation *console)
{
	int loop;
	int loop2;
	int Screenlines;
	SDL_Rect DestRect;

	if (!console)
		return;

	/* Due to the Blits, the update is not very fast: So only update if it's worth it */
	if (!CON_isOpen(console))
		return;

	Screenlines = console->ConsoleSurface->h / console->FontHeight;

	SDL_FillRect(console->ConsoleSurface, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, console->ConsoleAlpha));

	/* draw the background image if there is one */
	if (console->BackgroundImage)
	{
		DestRect.x = console->BackX;
		DestRect.y = console->BackY;
		DestRect.w = console->BackgroundImage->w;
		DestRect.h = console->BackgroundImage->h;
		SDL_BlitSurface(console->BackgroundImage, NULL, console->ConsoleSurface, &DestRect);
	}

	/*	Draw the text from the back buffers, calculate in the scrollback from the user
		draw text from last but second line to top
		loop: for every line in the history
		loop2: draws the scroll indicators to the line above the Commandline
	*/
	for (loop = 0; loop < Screenlines - 1 && loop < console->LineBuffer - console->ConsoleScrollBack; loop++)
	{
		if (console->ConsoleScrollBack != 0 && loop == 0)
			for (loop2 = 0; loop2 < (console->VChars / 5) + 1; loop2++)
				BF_RenderText(console->Font, CON_SCROLL_INDICATOR, console->ConsoleSurface, CON_CHAR_BORDER + (loop2 * 5 * console->FontWidth), (Screenlines - loop - 2) * console->FontHeight);
		else
			BF_RenderText(console->Font, console->ConsoleLines[console->ConsoleScrollBack + loop], console->ConsoleSurface, CON_CHAR_BORDER, (Screenlines - loop - 2) * console->FontHeight);
	}
}

void CON_UpdateOffset(ConsoleInformation *console)
{
	float pct = 0;

	if (!console)
		return;

	switch (console->Visible)
	{
	case CON_CLOSING:
		pct = (SDL_GetTicks() - console->RaiseTicks) / (float)CON_OPENCLOSE_SPEED;
		console->RaiseOffset = (int)((1.0 - pct) * console->ConsoleSurface->h);
		if (console->RaiseOffset <= 0)
		{
			console->RaiseOffset = 0;
			console->Visible = CON_CLOSED;
		}
		break;
	case CON_OPENING:
		pct = (SDL_GetTicks() - console->RaiseTicks) / (float)CON_OPENCLOSE_SPEED;
		console->RaiseOffset = (int)(pct * console->ConsoleSurface->h);
		if (console->RaiseOffset >= console->ConsoleSurface->h)
		{
			console->RaiseOffset = console->ConsoleSurface->h;
			console->Visible = CON_OPEN;
		}
		break;
	case CON_OPEN:
	case CON_CLOSED:
		break;
	}
}

/* Draws the console buffer to the screen if the console is "visible" */
void CON_DrawConsole(ConsoleInformation *console)
{
	SDL_Rect DestRect;
	SDL_Rect SrcRect;

	if (!console)
		return;

	/* only draw if console is visible: here this means, that the console is not CON_CLOSED */
	if (console->Visible == CON_CLOSED)
		return;

	/* Update the scrolling offset */
	CON_UpdateOffset(console);

	/* Update the command line since it has a blinking cursor */
	DrawCommandLine();

	SrcRect.x = 0;
	SrcRect.y = console->ConsoleSurface->h - console->RaiseOffset;
	SrcRect.w = console->ConsoleSurface->w;
	SrcRect.h = console->RaiseOffset;

	/* Setup the rect the console is being blitted into based on the output screen */
	DestRect.x = console->DispX;
	DestRect.y = console->DispY;
	DestRect.w = console->ConsoleSurface->w;
	DestRect.h = console->ConsoleSurface->h;

	SDL_BlitSurface(console->ConsoleSurface, &SrcRect, console->OutputSurface, &DestRect);
}

ConsoleInformation *CON_Init(const char *FontName, SDL_Surface *OutputSurface, int lines, SDL_Rect rect)
{
	int loop;
	SDL_Surface *Temp;
	ConsoleInformation *newinfo;

	/* Create a new console struct and init it. */
	if ((newinfo = (ConsoleInformation *)malloc(sizeof(ConsoleInformation))) == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not allocate the space for a new console info struct.");
		return NULL;
	}
	newinfo->Visible = CON_CLOSED;
	newinfo->RaiseOffset = 0;
	newinfo->RaiseTicks = 0;
	newinfo->ConsoleLines = NULL;
	newinfo->CommandLines = NULL;
	newinfo->TotalConsoleLines = 0;
	newinfo->ConsoleScrollBack = 0;
	newinfo->TotalCommands = 0;
	newinfo->BackgroundImage = NULL;
	newinfo->ConsoleAlpha = SDL_ALPHA_OPAQUE;
	newinfo->Offset = 0;
	newinfo->InsMode = 1;
	newinfo->CursorPos = 0;
	newinfo->CommandScrollBack = 0;
	newinfo->OutputSurface = OutputSurface;
	newinfo->Prompt = CON_DEFAULT_PROMPT;
	newinfo->ToggleKey = CON_DEFAULT_TOGGLEKEY;

	CON_SetExecuteFunction(newinfo, Default_CmdFunction);
	CON_SetTabCompletion(newinfo, Default_TabFunction);

	/* Load the consoles font */
	if (NULL == (newinfo->Font = BF_OpenFont(FontName, OutputSurface->format)))
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not load font '%s' for the console!", FontName);
		return NULL;
	}

	newinfo->FontHeight = newinfo->Font->CharHeight;
	newinfo->FontWidth = newinfo->Font->CharWidth;

	/* make sure that the size of the console is valid */
	if (rect.w > newinfo->OutputSurface->w || rect.w < newinfo->FontWidth * 32)
		rect.w = newinfo->OutputSurface->w;
	if (rect.h > newinfo->OutputSurface->h || rect.h < newinfo->FontHeight)
		rect.h = newinfo->OutputSurface->h;
	if (rect.x < 0 || rect.x > newinfo->OutputSurface->w - rect.w)
		newinfo->DispX = 0;
	else
		newinfo->DispX = rect.x;
	if (rect.y < 0 || rect.y > newinfo->OutputSurface->h - rect.h)
		newinfo->DispY = 0;
	else
		newinfo->DispY = rect.y;

	/* load the console surface */
	Temp = SDL_CreateRGBSurface(newinfo->OutputSurface->format->format, rect.w, rect.h,
								newinfo->OutputSurface->format->BitsPerPixel,
								newinfo->OutputSurface->format->Rmask,
								newinfo->OutputSurface->format->Gmask,
								newinfo->OutputSurface->format->Bmask,
								newinfo->OutputSurface->format->Amask);

	if (Temp == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't create the ConsoleSurface.");
		return NULL;
	}
	newinfo->ConsoleSurface = SDL_ConvertSurface(Temp, newinfo->OutputSurface->format, 0);
	SDL_FreeSurface(Temp);

	SDL_FillRect(newinfo->ConsoleSurface, NULL, SDL_MapRGBA(newinfo->ConsoleSurface->format, 0, 0, 0, newinfo->ConsoleAlpha));

	/* Load the dirty rectangle for user input */
	Temp = SDL_CreateRGBSurface(newinfo->OutputSurface->format->format, rect.w, newinfo->FontHeight,
								newinfo->OutputSurface->format->BitsPerPixel, 0, 0, 0, 0);
	if (Temp == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't create the InputBackground");
		return NULL;
	}
	newinfo->InputBackground = SDL_ConvertSurface(Temp, newinfo->OutputSurface->format, 0);
	// newinfo->InputBackground = SDL_DisplayFormat(Temp);
	SDL_FreeSurface(Temp);
	SDL_FillRect(newinfo->InputBackground, NULL, SDL_MapRGBA(newinfo->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));

	/* calculate the number of visible characters in the command line */
	newinfo->VChars = (rect.w - CON_CHAR_BORDER) / newinfo->FontWidth;
	if (newinfo->VChars > CON_CHARS_PER_LINE)
		newinfo->VChars = CON_CHARS_PER_LINE;

	newinfo->LineBuffer = lines;

	newinfo->ConsoleLines = (char **)malloc(sizeof(char *) * newinfo->LineBuffer);
	newinfo->CommandLines = (char **)malloc(sizeof(char *) * newinfo->LineBuffer);
	for (loop = 0; loop <= newinfo->LineBuffer - 1; loop++)
	{
		newinfo->ConsoleLines[loop] = (char *)calloc(CON_CHARS_PER_LINE + 1, sizeof(char));
		newinfo->CommandLines[loop] = (char *)calloc(CON_CHARS_PER_LINE + 1, sizeof(char));
	}
	memset(newinfo->Command, 0, sizeof(newinfo->Command));
	memset(newinfo->LCommand, 0, sizeof(newinfo->LCommand));
	memset(newinfo->RCommand, 0, sizeof(newinfo->RCommand));
	memset(newinfo->VCommand, 0, sizeof(newinfo->VCommand));

	CON_Out(newinfo, "Console initialised.");
	CON_NewLineConsole(newinfo);

	return newinfo;
}

/* Makes the console visible */
void CON_Show(ConsoleInformation *console)
{
	if (console)
	{
		console->Visible = CON_OPENING;
		console->RaiseTicks = SDL_GetTicks();
		CON_UpdateConsole(console);
	}
}

/* Hides the console (make it invisible) */
void CON_Hide(ConsoleInformation *console)
{
	if (console)
	{
		console->Visible = CON_CLOSING;
		console->RaiseTicks = SDL_GetTicks();
	}
}

/* tells wether the console is visible or not */
int CON_isOpen(ConsoleInformation *console)
{
	if (!console)
		return 0;
	return ((console->Visible == CON_OPEN) || (console->Visible == CON_OPENING));
}

/* Frees all the memory loaded by the console */
void CON_Destroy(ConsoleInformation *console)
{
	int i;

	if (!console)
		return;

	BF_CloseFont(console->Font);

	for (i = 0; i <= console->LineBuffer - 1; i++)
	{
		free(console->ConsoleLines[i]);
		free(console->CommandLines[i]);
	}
	free(console->ConsoleLines);
	free(console->CommandLines);

	console->ConsoleLines = NULL;
	console->CommandLines = NULL;
	free(console);
}

/* Increments the console lines */
void CON_NewLineConsole(ConsoleInformation *console)
{
	int loop;
	char *temp;

	if (!console)
		return;

	temp = console->ConsoleLines[console->LineBuffer - 1];

	for (loop = console->LineBuffer - 1; loop > 0; loop--)
		console->ConsoleLines[loop] = console->ConsoleLines[loop - 1];

	console->ConsoleLines[0] = temp;

	memset(console->ConsoleLines[0], 0, CON_CHARS_PER_LINE + 1);
	if (console->TotalConsoleLines < console->LineBuffer - 1)
		console->TotalConsoleLines++;

	/* Now adjust the ConsoleScrollBack
	   dont scroll if not at bottom */
	if (console->ConsoleScrollBack != 0)
		console->ConsoleScrollBack++;
	/* boundaries */
	if (console->ConsoleScrollBack > console->LineBuffer - 1)
		console->ConsoleScrollBack = console->LineBuffer - 1;
}

/* Increments the command lines */
void CON_AddHistoryCommand(ConsoleInformation *console, const char *command)
{
	if (!console)
		return;

	char *temp;
	temp = console->CommandLines[console->LineBuffer - 1];

	for (int loop = console->LineBuffer - 1; loop > 0; loop--)
		console->CommandLines[loop] = console->CommandLines[loop - 1];

	console->CommandLines[0] = temp;

	memset(console->CommandLines[0], 0, CON_CHARS_PER_LINE + 1);
	strncpy(Topmost->CommandLines[0], command, CON_CHARS_PER_LINE + 1);

	if (console->TotalCommands < console->LineBuffer - 1)
		console->TotalCommands++;
}

/* Draws the command line the user is typing in to the screen */
/* completely rewritten by C.Wacha */
void DrawCommandLine()
{
	SDL_Rect rect;
	int commandbuffer;
	static Uint32 NextBlinkTime = 0; /* time the consoles cursor blinks again */
	static int LastCursorPos = 0;	 /* Last Cursor Position */
	static int Blink = 0;			 /* Is the cursor currently blinking */

	if (!Topmost)
		return;

	commandbuffer = Topmost->VChars - u8strlen(Topmost->Prompt) - 1; /*  -1 to make cursor visible */

	/* calculate display offset from current cursor position */
	if (Topmost->Offset < Topmost->CursorPos - commandbuffer)
		Topmost->Offset = Topmost->CursorPos - commandbuffer;
	if (Topmost->Offset > Topmost->CursorPos)
		Topmost->Offset = Topmost->CursorPos;

	/* add prompt and the visible part of the command */
	snprintf(Topmost->VCommand, CON_CHARS_PER_LINE + 1, "%s%s", Topmost->Prompt, &Topmost->Command[Topmost->Offset]);

	/* now display the result */

	/* first of all restore InputBackground */
	rect.x = 0;
	rect.y = Topmost->ConsoleSurface->h - Topmost->FontHeight;
	rect.w = Topmost->InputBackground->w;
	rect.h = Topmost->InputBackground->h;
	SDL_BlitSurface(Topmost->InputBackground, NULL, Topmost->ConsoleSurface, &rect);

	/* now add the text */
	BF_RenderText(Topmost->Font, Topmost->VCommand, Topmost->ConsoleSurface, CON_CHAR_BORDER, Topmost->ConsoleSurface->h - Topmost->FontHeight);

	/* at last add the cursor
	   check if the blink period is over */
	if (SDL_GetTicks() > NextBlinkTime)
	{
		NextBlinkTime = SDL_GetTicks() + CON_BLINK_RATE;
		Blink = 1 - Blink;
	}

	/* check if cursor has moved - if yes display cursor anyway */
	if (Topmost->CursorPos != LastCursorPos)
	{
		LastCursorPos = Topmost->CursorPos;
		NextBlinkTime = SDL_GetTicks() + CON_BLINK_RATE;
		Blink = 1;
	}

	if (Blink)
	{
		int logical_position = strlen(Topmost->Prompt) + Topmost->CursorPos - Topmost->Offset;
		int x = CON_CHAR_BORDER + Topmost->FontWidth * logical_position;
		if (Topmost->InsMode)
			BF_RenderText(Topmost->Font, CON_INS_CURSOR, Topmost->ConsoleSurface, x, Topmost->ConsoleSurface->h - Topmost->FontHeight);
		else
			BF_RenderText(Topmost->Font, CON_OVR_CURSOR, Topmost->ConsoleSurface, x, Topmost->ConsoleSurface->h - Topmost->FontHeight);
	}
}

/* Outputs text to the console (in game), up to CON_CHARS_PER_LINE chars can be entered */
void CON_Out(ConsoleInformation *console, const char *str, ...)
{
	va_list marker;

	char temp[CON_CHARS_PER_LINE + 1];
	char *ptemp;

	if (!console)
		return;

	va_start(marker, str);
	vsnprintf(temp, CON_CHARS_PER_LINE, str, marker);
	va_end(marker);

	ptemp = temp;

	/* temp now contains the complete string we want to output
	   the only problem is that temp is maybe longer than the console
	   width so we have to cut it into several pieces */

	if (console->ConsoleLines)
	{
		while (strlen(ptemp) > console->VChars)
		{
			CON_NewLineConsole(console);
			strncpy(console->ConsoleLines[0], ptemp, console->VChars);
			console->ConsoleLines[0][console->VChars] = '\0';
			ptemp = &ptemp[console->VChars];
		}
		CON_NewLineConsole(console);
		strncpy(console->ConsoleLines[0], ptemp, console->VChars);
		console->ConsoleLines[0][console->VChars] = '\0';
		CON_UpdateConsole(console);
	}

	/* And print to stdout */
	/* printf("%s\n", temp); */
}

/* Sets the alpha level of the console, 0 turns off alpha blending */
void CON_Alpha(ConsoleInformation *console, unsigned char alpha)
{
	if (!console)
		return;

	/* store alpha as state! */
	console->ConsoleAlpha = alpha;
	SDL_SetSurfaceAlphaMod(console->ConsoleSurface, alpha);
}

/* Adds  background image to the console, x and y based on consoles x and y */
int CON_Background(ConsoleInformation *console, const char *image, int x, int y)
{
	SDL_Surface *temp;
	SDL_Rect backgroundsrc, backgrounddest;

	if (!console)
		return 1;

	/* Free the background from the console */
	if (console->BackgroundImage != NULL)
	{
		SDL_FreeSurface(console->BackgroundImage);
		console->BackgroundImage = NULL;
	}

	if (image == NULL)
	{
		SDL_FillRect(console->InputBackground, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
		return 0;
	}

	/* Load a new background */
#ifdef HAVE_SDLIMAGE
	temp = IMG_Load(image);
#else
	temp = SDL_LoadBMP(image);
#endif
	if (!temp)
	{
		CON_Out(console, "Cannot load background %s.", image);
		return 1;
	}

	console->BackgroundImage = SDL_ConvertSurface(temp, console->OutputSurface->format, 0);
	SDL_FreeSurface(temp);
	console->BackX = x;
	console->BackY = y;

	backgroundsrc.x = 0;
	backgroundsrc.y = console->ConsoleSurface->h - console->FontHeight - console->BackY;
	backgroundsrc.w = console->BackgroundImage->w;
	backgroundsrc.h = console->InputBackground->h;

	backgrounddest.x = console->BackX;
	backgrounddest.y = 0;
	backgrounddest.w = console->BackgroundImage->w;
	backgrounddest.h = console->FontHeight;

	SDL_FillRect(console->InputBackground, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
	SDL_BlitSurface(console->BackgroundImage, &backgroundsrc, console->InputBackground, &backgrounddest);

	CON_UpdateConsole(console);
	return 0;
}

/* takes a new x and y of the top left of the console window */
void CON_Position(ConsoleInformation *console, int x, int y)
{
	if (!console)
		return;

	if (x < 0 || x > console->OutputSurface->w - console->ConsoleSurface->w)
		console->DispX = 0;
	else
		console->DispX = x;

	if (y < 0 || y > console->OutputSurface->h - console->ConsoleSurface->h)
		console->DispY = 0;
	else
		console->DispY = y;
}

/* resizes the console, has to reset alot of stuff
 * returns 1 on error */
int CON_Resize(ConsoleInformation *console, SDL_Rect rect)
{
	SDL_Surface *Temp;
	SDL_Rect backgroundsrc, backgrounddest;

	if (!console)
		return 1;

	/* make sure that the size of the console is valid */
	if (rect.w > console->OutputSurface->w || rect.w < console->FontWidth * 32)
		rect.w = console->OutputSurface->w;
	if (rect.h > console->OutputSurface->h || rect.h < console->FontHeight)
		rect.h = console->OutputSurface->h;
	if (rect.x < 0 || rect.x > console->OutputSurface->w - rect.w)
		console->DispX = 0;
	else
		console->DispX = rect.x;
	if (rect.y < 0 || rect.y > console->OutputSurface->h - rect.h)
		console->DispY = 0;
	else
		console->DispY = rect.y;

	/* load the console surface */
	SDL_FreeSurface(console->ConsoleSurface);
	Temp = SDL_CreateRGBSurface(console->OutputSurface->format->format, rect.w, rect.h,
								console->OutputSurface->format->BitsPerPixel,
								console->OutputSurface->format->Rmask,
								console->OutputSurface->format->Gmask,
								console->OutputSurface->format->Bmask,
								console->OutputSurface->format->Amask);
	if (Temp == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't create the console->ConsoleSurface");
		return 1;
	}
	console->ConsoleSurface = SDL_ConvertSurface(Temp, console->OutputSurface->format, 0);
	SDL_FreeSurface(Temp);

	/* Load the dirty rectangle for user input */
	SDL_FreeSurface(console->InputBackground);
	Temp = SDL_CreateRGBSurface(console->OutputSurface->format->format, rect.w, console->FontHeight,
								console->OutputSurface->format->BitsPerPixel, 0, 0, 0, 0);
	if (Temp == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't create the input background");
		return 1;
	}
	console->InputBackground = SDL_ConvertSurface(Temp, console->OutputSurface->format, 0);
	SDL_FreeSurface(Temp);

	/* Now reset some stuff dependent on the previous size */
	console->ConsoleScrollBack = 0;

	/* Reload the background image (for the input text area) in the console */
	if (console->BackgroundImage)
	{
		backgroundsrc.x = 0;
		backgroundsrc.y = console->ConsoleSurface->h - console->FontHeight - console->BackY;
		backgroundsrc.w = console->BackgroundImage->w;
		backgroundsrc.h = console->InputBackground->h;

		backgrounddest.x = console->BackX;
		backgrounddest.y = 0;
		backgrounddest.w = console->BackgroundImage->w;
		backgrounddest.h = console->FontHeight;

		SDL_FillRect(console->InputBackground, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
		SDL_BlitSurface(console->BackgroundImage, &backgroundsrc, console->InputBackground, &backgrounddest);
	}

	/* restore the alpha level */
	CON_Alpha(console, console->ConsoleAlpha);

	/* re-calculate the number of visible characters in the command line */
	console->VChars = (rect.w - CON_CHAR_BORDER) / console->FontWidth;
	if (console->VChars > CON_CHARS_PER_LINE)
		console->VChars = CON_CHARS_PER_LINE;

	CON_UpdateConsole(console);
	return 0;
}

/* Transfers the console to another screen surface, and adjusts size */
int CON_Transfer(ConsoleInformation *console, SDL_Surface *NewOutputSurface, SDL_Rect rect)
{
	if (!console)
		return 1;

	console->OutputSurface = NewOutputSurface;

	return (CON_Resize(console, rect));
}

/* Sets the topmost console for input */
void CON_Topmost(ConsoleInformation *console)
{
	SDL_Rect rect;

	if (!console)
		return;

	/* Make sure the blinking cursor is gone */
	if (Topmost)
	{
		rect.x = 0;
		rect.y = Topmost->ConsoleSurface->h - Topmost->FontHeight;
		rect.w = Topmost->InputBackground->w;
		rect.h = Topmost->InputBackground->h;
		SDL_BlitSurface(Topmost->InputBackground, NULL, Topmost->ConsoleSurface, &rect);
		BF_RenderText(Topmost->Font, Topmost->VCommand, Topmost->ConsoleSurface, CON_CHAR_BORDER, Topmost->ConsoleSurface->h - Topmost->FontHeight);
	}
	Topmost = console;
	if (console)
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
}

/* Sets the Prompt for console */
void CON_SetPrompt(ConsoleInformation *console, char *newprompt)
{
	if (!console)
		return;

	/* check length so we can still see at least 1 char :-) */
	if (strlen(newprompt) < console->VChars)
		console->Prompt = strdup(newprompt);
	else
		CON_Out(console, "prompt too long. (max. %i chars)", console->VChars - 1);
}

/* Sets the key that deactivates (hides) the console. */
void CON_SetToggleKey(ConsoleInformation *console, SDL_Scancode key)
{
	if (console)
		console->ToggleKey = key;
}

/* Executes the command entered */
void CON_Execute(ConsoleInformation *console, char *command)
{
	if (console)
		console->CmdFunction(console, command);
}

void CON_SetExecuteFunction(ConsoleInformation *console, void (*CmdFunction)(ConsoleInformation *console2, char *command))
{
	if (console)
		console->CmdFunction = CmdFunction;
}

void Default_CmdFunction(ConsoleInformation *console, char *command)
{
	CON_Out(console, "     No CommandFunction registered");
	CON_Out(console, "     use 'CON_SetExecuteFunction' to register one");
	CON_Out(console, " ");
	CON_Out(console, "Unknown Command \"%s\"", command);
}

void CON_SetTabCompletion(ConsoleInformation *console, char *(*TabFunction)(char *command))
{
	if (console)
		console->TabFunction = TabFunction;
}

void CON_TabCompletion(ConsoleInformation *console)
{
	int i, j;
	char *command;

	if (!console)
		return;

	command = strdup(console->LCommand);
	command = console->TabFunction(command);

	if (!command)
		return; /* no tab completion took place so return silently */

	/*	command now contains the tabcompleted string. check for correct size
		since the string has to fit into the commandline it can have a maximum length of
		CON_CHARS_PER_LINE = commandlength + space + cursor
		=> commandlength = CON_CHARS_PER_LINE - 2
	*/
	j = strlen(command);
	if (j + 2 > CON_CHARS_PER_LINE)
		j = CON_CHARS_PER_LINE - 2;

	memset(console->LCommand, 0, CON_CHARS_PER_LINE + 1);
	console->CursorPos = 0;

	for (i = 0; i < j; i++)
	{
		console->CursorPos++;
		console->LCommand[i] = command[i];
	}
	/* add a trailing space */
	console->CursorPos++;
	console->LCommand[j] = ' ';
	console->LCommand[j + 1] = '\0';

	Assemble_Command(console);
}

char *Default_TabFunction(char *command)
{
	CON_Out(Topmost, "     No TabFunction registered");
	CON_Out(Topmost, "     use 'CON_SetTabCompletion' to register one");
	CON_Out(Topmost, " ");
	return NULL;
}

void Cursor_Left(ConsoleInformation *console)
{
	char temp[sizeof(Topmost->RCommand)];

	if (Topmost->CursorPos > 0)
	{
		Topmost->CursorPos--;
		strcpy(temp, Topmost->RCommand);
		size_t last = strlen(Topmost->LCommand);
		u8_dec(Topmost->LCommand, &last);
		// strcpy(Topmost->RCommand, &Topmost->LCommand[strlen(Topmost->LCommand) - 1]);
		strcpy(Topmost->RCommand, &Topmost->LCommand[last]);
		strcat(Topmost->RCommand, temp);
		Topmost->LCommand[last] = '\0';
		// CON_Out(Topmost, "L:%s | R:%s", Topmost->LCommand, Topmost->RCommand);
	}
}

void Cursor_Right(ConsoleInformation *console)
{
	char temp[sizeof(Topmost->LCommand)];

	if (Topmost->CursorPos < u8strlen(Topmost->Command))
	{
		Topmost->CursorPos++;
		strcpy(temp, Topmost->LCommand);
		size_t idx = 0;
		u8_inc(Topmost->RCommand, &idx);
		// FIXME: this is not yet good...
		if (strlen(Topmost->LCommand) + idx < sizeof(Topmost->LCommand))
			strncat(Topmost->LCommand, Topmost->RCommand, idx);
		// snprintf(Topmost->LCommand, strlen(Topmost->LCommand) + idx + 1, "%s%s", temp, Topmost->RCommand);
		//  snprintf(Topmost->LCommand, strlen(Topmost->LCommand) + 2, "%s%c", temp, Topmost->RCommand[0]);

		strcpy(temp, Topmost->RCommand);
		strcpy(Topmost->RCommand, &temp[idx]);
		// CON_Out(Topmost, "L:%s | R:%s", Topmost->LCommand, Topmost->RCommand);
	}
}

void Cursor_Home(ConsoleInformation *console)
{
	Topmost->CursorPos = 0;
	strcpy(Topmost->RCommand, Topmost->Command);
	memset(Topmost->LCommand, 0, sizeof(Topmost->LCommand));
	// CON_Out(Topmost, "L:%s | R:%s", Topmost->LCommand, Topmost->RCommand);
}

void Cursor_End(ConsoleInformation *console)
{
	Topmost->CursorPos = u8strlen(Topmost->Command);
	strcpy(Topmost->LCommand, Topmost->Command);
	memset(Topmost->RCommand, 0, sizeof(Topmost->RCommand));
	// CON_Out(Topmost, "L:%s | R:%s", Topmost->LCommand, Topmost->RCommand);
}

void Cursor_Del(ConsoleInformation *console)
{
	char temp[CON_CHARS_PER_LINE + 1];

	if (strlen(Topmost->RCommand) > 0)
	{
		size_t idx = 0;
		u8_inc(Topmost->RCommand, &idx);
		// int length = u8next(Topmost->RCommand, NULL);
		strcpy(temp, Topmost->RCommand);
		strcpy(Topmost->RCommand, &temp[idx]);
		Assemble_Command(console);
	}
}

void Cursor_BSpace(ConsoleInformation *console)
{
	if (Topmost->CursorPos > 0)
	{
		Topmost->CursorPos--;
		Topmost->Offset--;
		if (Topmost->Offset < 0)
			Topmost->Offset = 0;
		size_t idx = strlen(Topmost->LCommand);
		u8_dec(Topmost->LCommand, &idx);
		Topmost->LCommand[idx] = '\0';
		Assemble_Command(console);
	}
}

void Cursor_Add(ConsoleInformation *console, SDL_TextInputEvent *event)
{
	char *text = event->text;
	size_t text_length = u8strlen(text);

	/* Again: the commandline has to hold the command and the cursor (+1) */
	if (strlen(Topmost->Command) + 1 < CON_CHARS_PER_LINE && text_length > 0)
	{
		Topmost->CursorPos += text_length;
		// strcpy(temp, Topmost->LCommand);
		// snprintf(Topmost->LCommand, strlen(Topmost->LCommand) + 2, "%s%s", temp, text);
		strncat(Topmost->LCommand, text, sizeof(Topmost->LCommand) - strlen(Topmost->LCommand) - 1);
		Assemble_Command(console);
	}
}

void Clear_Command(ConsoleInformation *console)
{
	Topmost->CursorPos = 0;
	memset(Topmost->VCommand, 0, sizeof(Topmost->VCommand));
	memset(Topmost->Command, 0, sizeof(Topmost->Command));
	memset(Topmost->LCommand, 0, sizeof(Topmost->LCommand));
	memset(Topmost->RCommand, 0, sizeof(Topmost->RCommand));
}

void Assemble_Command(ConsoleInformation *console)
{
	/* Concatenate the left and right side to command */
	snprintf(Topmost->Command, sizeof(Topmost->Command), "%s%s", Topmost->LCommand, Topmost->RCommand);
}

void Clear_History(ConsoleInformation *console)
{
	int loop;

	for (loop = 0; loop <= console->LineBuffer - 1; loop++)
		memset(console->ConsoleLines[loop], 0, CON_CHARS_PER_LINE + 1);
}

void Command_Up(ConsoleInformation *console)
{
	if (console->CommandScrollBack < console->TotalCommands - 1)
	{
		/* move back a line in the command strings and copy the command to the current input string */
		console->CommandScrollBack++;
		/* I want to know if my string handling REALLY works :-) */
		/* memset(console->RCommand, 0, CON_CHARS_PER_LINE);
		memset(console->LCommand, 0, CON_CHARS_PER_LINE); */
		console->RCommand[0] = '\0';
		console->LCommand[0] = '\0';

		console->Offset = 0;
		strcpy(console->LCommand, console->CommandLines[console->CommandScrollBack]);
		console->CursorPos = u8strlen(console->CommandLines[console->CommandScrollBack]);
		Assemble_Command(console);
	}
}

void Command_Down(ConsoleInformation *console)
{
	if (console->CommandScrollBack > -1)
	{
		/* move forward a line in the command strings and copy the command to the current input string */
		console->CommandScrollBack--;
		/* I want to know if my string handling REALLY works :-) */
		/* memset(console->RCommand, 0, CON_CHARS_PER_LINE);
		memset(console->LCommand, 0, CON_CHARS_PER_LINE); */
		console->RCommand[0] = '\0';
		console->LCommand[0] = '\0';

		console->Offset = 0;
		if (console->CommandScrollBack > -1)
			strcpy(console->LCommand, console->CommandLines[console->CommandScrollBack]);
		console->CursorPos = u8strlen(console->LCommand);
		Assemble_Command(console);
	}
}
