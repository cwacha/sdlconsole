/* This is an example of the console code for SDL
 * Garrett Banuk (mongoose@mongeese.org)
 * Clemens Wacha (reflex-2000@gmx.net)
 */

/*	How to read this file:

	The important steps are marked with STEP 1 to 4. They are all in the main()
	function. The Interaction stuff for the console can be found in the ProcessEvents()
	function.

*/

#ifdef HAVE_OPENGL
#include <GL/glut.h>
#endif /* HAVE_OPENGL */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "SDL_console.h"
#include "DT_drawtext.h"
#include "ConsoleExample.h"
#include "split.h"

int done = 0;
int TextDemo = 0;
int LargeFont;

#define CONSOLE_N 3
ConsoleInformation *Consoles[CONSOLE_N]; /* Pointers to all the consoles */

void Log(void *userdata, int category, SDL_LogPriority priority, const char *message)
{

	FILE *f;
	f = fopen("error.log", "a+");
	if (f != NULL)
	{
		fprintf(f, "%s\n", message);
		fclose(f);
	}
	printf("%s\n", message);

	if (Consoles[0])
		CON_Out(Consoles[0], message);
}

int main(int argc, char **argv)
{
	SDL_Renderer *sdlRenderer;
	SDL_Texture *sdlTexture;
	SDL_Surface *Screen;
	int now = 0, then = 0;
	int frames = 0;
	int i;
	char framerate[30];
	SDL_Rect Con_rect;

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
	SDL_LogSetOutputFunction(Log, NULL);

	/* init the graphics */
	if (Init(&sdlRenderer, &sdlTexture, &Screen, argc, argv))
		return 1;

	/* STEP 1: Init the consoles */
	Con_rect.x = 0;
	Con_rect.y = 0;
	Con_rect.w = 640;
	Con_rect.h = 300;
	if ((Consoles[0] = CON_Init("ExportedFont.bmp", Screen, 100, Con_rect)) == NULL)
		return 1;

	Con_rect.x = 350;
	Con_rect.y = 20;
	Con_rect.w = Con_rect.h = 200;
	if ((Consoles[1] = CON_Init("ConsoleFont2.bmp", Screen, 100, Con_rect)) == NULL)
		return 1;

	Con_rect.x = 340;
	Con_rect.y = 280;
	Con_rect.w = 300;
	Con_rect.h = 200;
	if ((Consoles[2] = CON_Init("ConsoleFont2.bmp", Screen, 100, Con_rect)) == NULL)
		return 1;

	SDL_LogError(SDL_LOG_CATEGORY_ERROR, " !\"#$%%&\'()*+,-./0123456789:;<=>?");
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "`abcdefghijklmnopqrstuvwxyz{|}~");

	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "  ! \" # $ %% & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?");
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _ ");
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ ");

	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "custom log function initialized");

	/* STEP 2: Attach the Command handling function to the consoles. Remark that every
	   console can have its own command handler */
	CON_SetExecuteFunction(Consoles[0], Command_Handler);
	CON_SetExecuteFunction(Consoles[1], Command_Handler);
	CON_SetExecuteFunction(Consoles[2], Command_Handler);

	// CON_SetToggleKey(Consoles[0], SDL_SCANCODE_GRAVE);
	ListCommands(Consoles[0]);
	CON_Show(Consoles[0]);
	CON_Topmost(Consoles[0]);

	/* Heres another font for the text demo */
	LargeFont = DT_LoadFont("LargeFont.gif", 0);

	/* Main execution loop */
	while (!done)
	{
		ProcessEvents();

		/* wipe the screen clean with a blue fill and draw the console if its down */
		SDL_FillRect(Screen, NULL, SDL_MapRGB(Screen->format, 0, 0, 255));

		HelpText(Screen);
		if (TextDemo)
			RandText(Screen);

		/* STEP 3: in your video loop, call the drawing function */
		for (i = 0; i < CONSOLE_N; i++)
			CON_DrawConsole(Consoles[i]);

		/* print the framerate */
		frames++;
		now = SDL_GetTicks();
		if (now > then + 250)
		{
			sprintf(framerate, "%7.2f fps", ((double)frames * 1000) / (now - then));
			then = now;
			frames = 0;
		}
		DT_DrawText(framerate, Screen, 1, 1, Screen->h - 40);

		SDL_UpdateTexture(sdlTexture, NULL, Screen->pixels, Screen->pitch);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);
	}

	/* STEP 4: delete your consoles before quitting */
	for (i = 0; i < CONSOLE_N; i++)
		CON_Destroy(Consoles[i]);

	return 0;
}

/* Initialise the graphics */
int Init(SDL_Renderer **sdlRenderer, SDL_Texture **sdlTexture, SDL_Surface **Screen, int argc, char **argv)
{
	Uint32 SetVideoFlags = SDL_WINDOW_OPENGL;
	int width = 640, height = 480, bpp = 32;
	int loop;

	for (loop = 1; loop < argc; loop++)
	{
		if (strcmp(argv[loop], "-fullscreen") == 0)
			SetVideoFlags |= SDL_WINDOW_FULLSCREEN;
		else if (strcmp(argv[loop], "-width") == 0)
			width = atoi(argv[++loop]);
		else if (strcmp(argv[loop], "-height") == 0)
			height = atoi(argv[++loop]);
		else if (strcmp(argv[loop], "-bpp") == 0)
			bpp = atoi(argv[++loop]);
		else if (strcmp(argv[loop], "-sw") == 0)
			SetVideoFlags |= SDL_SWSURFACE;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "*Error* Couldn't initialize SDL: %s", SDL_GetError());
		return 1;
	}

	SDL_Window *sdlWindow = SDL_CreateWindow("My Game Window",
											 SDL_WINDOWPOS_UNDEFINED,
											 SDL_WINDOWPOS_UNDEFINED,
											 width, height,
											 SetVideoFlags);

	*sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
	// SDL_SetRenderDrawBlendMode(*sdlRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(*sdlRenderer, 0, 0, 0, 255);

	Uint32 Rmask, Gmask, Bmask, Amask;
	Uint32 PixelFormat;
	switch (bpp)
	{
	case 8:
		PixelFormat = SDL_PIXELFORMAT_RGB332;
		break;
	case 15:
		PixelFormat = SDL_PIXELFORMAT_RGB555;
		break;
	case 16:
		PixelFormat = SDL_PIXELFORMAT_ARGB4444;
		break;
	case 24:
		PixelFormat = SDL_PIXELFORMAT_RGB24;
	case 32:
	default:
		PixelFormat = SDL_PIXELFORMAT_ARGB32;
		break;
	}
	SDL_PixelFormatEnumToMasks(PixelFormat, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
	*Screen = SDL_CreateRGBSurface(0, width, height, bpp, Rmask, Gmask, Bmask, Amask);
	*sdlTexture = SDL_CreateTexture(*sdlRenderer,
									PixelFormat,
									SDL_TEXTUREACCESS_STREAMING,
									width, height);

	atexit(SDL_Quit);
	return 0;
}

/* Processes all the incoming events */
void ProcessEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		/* Send the event to the (topmost) console and go on if the console
		 * could not handle the event */
		if (!CON_Events(&event))
			continue;

		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_1:
			case SDLK_2:
			case SDLK_3:
			case SDLK_4:
				if (event.key.keysym.mod & KMOD_CTRL)
				{
					/* Show/Hide consoles */
					if (CON_isOpen(Consoles[event.key.keysym.sym - SDLK_1]))
						CON_Hide(Consoles[event.key.keysym.sym - SDLK_1]);
					else
						CON_Show(Consoles[event.key.keysym.sym - SDLK_1]);
				}
				else if (event.key.keysym.mod & KMOD_ALT)
				{
					if ((event.key.keysym.sym - SDLK_1) == CONSOLE_N)
					{
						CON_Topmost(NULL);
						// SDL_EnableKeyRepeat(0, 0);
					}
					else
					{
						CON_Topmost(Consoles[event.key.keysym.sym - SDLK_1]);
						// SDL_EnableKeyRepeat(250, 30);
					}
				}
				break;
			case SDLK_ESCAPE:
				done = 1;
				break;
			default:
				// printf("%d\n", event.key.keysym.sym);
				break;
			}
			break;
		case SDL_QUIT:
			done = 1;
			break;
		default:
			break;
		}
	}
}

void Command_Handler(ConsoleInformation *console, char *command)
{
	int argc;
	char *argv[128];
	char *linecopy;

	linecopy = strdup(command);
	argc = splitline(argv, (sizeof argv) / (sizeof argv[0]), linecopy);
	if (!argc)
	{
		free(linecopy);
		return;
	}

	/*	This command handler is very stupid. Normally you would save your commands
		using an array of char* and pointers to functions. You will need something like this
		anyway if you want to make use of the tabfunction because you will have
		to implement the tabulator completion search function yourself

		To make myself clear: I use this construct in my own programs:

		typedef struct {
			char* commandname;
			int (*my_func)(int argc, char* argv[]);
		} command_t;

		command_t* cmd_table[] = {
								{ "quit", quit_function },
								{ "alpha", set_alpha },
								{ NULL, NULL }
								};

		and to search for a command:

		command_t* cmd;

		for (cmd = cmd_table; cmd->commandname; cmd++) {
			if(!strcasecmd(cmd->commandname, argv[0])) {
				command found, now start the function
				return cmd->myfunc;
			}
		}
		if we land here: command not found

		etc..

	*/

	if (!strcmp(argv[0], "quit"))
		KillProgram();
	else if (!strcmp(argv[0], "echo"))
		Echo(console, argc, argv);
	else if (!strcmp(argv[0], "drawtextdemo"))
		DrawTextDemo();
	else if (!strcmp(argv[0], "alpha"))
		AlphaChange(console, argc, argv);
	else if (!strcmp(argv[0], "background"))
		AddBackground(console, argc, argv);
	else if (!strcmp(argv[0], "move"))
		Move(console, argc, argv);
	else if (!strcmp(argv[0], "resize"))
		Resize(console, argc, argv);
	else if (!strcmp(argv[0], "help"))
		ListCommands(console);
	else if (!strcmp(argv[0], "prompt"))
		SetPrompt(console, argc, argv);
	else
		CON_Out(console, "%s: command not found", argv[0]);
}

/* call this to end the main loop */
void KillProgram()
{
	done = 1;
}

/* Prints the string you pass it into the console */
void Echo(ConsoleInformation *console, int argc, char *argv[])
{
	int i;

	for (i = 0; i < argc; i++)
	{
		CON_Out(console, "%s", argv[i]);
	}
}

/* This function toggles the draw text demo */
void DrawTextDemo()
{
	if (TextDemo == 0)
		TextDemo = 1;
	else
		TextDemo = 0;
}

/* This function displays a help message */
void HelpText(SDL_Surface *Screen)
{
	DT_DrawText("Show/Hide the consoles with Ctrl-1 to Ctrl-3", Screen, 0, 100, Screen->h - 30);
	DT_DrawText("Change input with Alt-1 to Alt-3. Alt-4 disables Input.", Screen, 0, 100, Screen->h - 20);
}

/* This function demonstrates the text drawing routines that
 * come with this console */
void RandText(SDL_Surface *Screen)
{
	DT_DrawText("This is an example of the DrawText routine", Screen, 0, 40, Screen->h - 50);
	DT_DrawText("This is an example of the DrawText routine", Screen, 0, 100, 300);
	DT_DrawText("This is an example of the DrawText routine", Screen, 0, 200, 400);
	DT_DrawText("This is an example of the DrawText routine", Screen, 0, 20, 20);
	DT_DrawText("This is an example of the DrawText routine", Screen, 0, 0, 0);
	DT_DrawText("This is an example of the DrawText routine", Screen, 0, 300, 5);
	DT_DrawText("This is an example of the DrawText routine", Screen, 0, 600, 90);

	/* Now show the large font */
	if (-1 != LargeFont)
	{
		DT_DrawText("Heres a large font (non-transparent)", Screen, LargeFont, 50, 60);
		DT_DrawText("Heres a large font (non-transparent)", Screen, LargeFont, 0, 170);
	}
}

/* lets the user change the alpha level */
void AlphaChange(ConsoleInformation *console, int argc, char *argv[])
{
	if (argc > 1)
	{
		CON_Alpha(console, atoi(argv[1]));
		CON_Out(console, "Alpha set to %s.", argv[1]);
	}
	else
		CON_Out(console, "usage: %s <alphavalue>", argv[0]);
}

/* Adds a background image */
void AddBackground(ConsoleInformation *console, int argc, char *argv[])
{
	int x, y;

	if (argc > 2)
	{
		x = atoi(argv[1]);
		y = atoi(argv[2]);
		CON_Background(console, "background.jpg", x, y);
	}
	else
	{
		CON_Background(console, NULL, 0, 0);
		CON_Out(console, "usage: %s <x> <y>", argv[0]);
	}
}

/* Move the console, takes and x and a y */
void Move(ConsoleInformation *console, int argc, char *argv[])
{
	int x, y;

	if (argc > 2)
	{
		x = atoi(argv[1]);
		y = atoi(argv[2]);
		CON_Position(console, x, y);
	}
	else
	{
		CON_Position(console, 0, 0);
		CON_Out(console, "usage: %s <x> <y>", argv[0]);
	}
}

/* resizes the console window, takes and x and y, and a width and height */
void Resize(ConsoleInformation *console, int argc, char *argv[])
{
	SDL_Rect rect;

	if (argc > 4)
	{
		rect.x = atoi(argv[1]);
		rect.y = atoi(argv[2]);
		rect.w = atoi(argv[3]);
		rect.h = atoi(argv[4]);
		CON_Resize(console, rect);
	}
	else
		CON_Out(console, "usage: %s <x> <y> <width> <height>", argv[0]);
}

void SetPrompt(ConsoleInformation *console, int argc, char *argv[])
{
	if (argc > 1)
		CON_SetPrompt(console, argv[1]);
	else
		CON_Out(console, "usage: %s <new_prompt>", argv[0]);
}

void ListCommands(ConsoleInformation *console)
{
	CON_Out(console, "help");
	CON_Out(console, "quit");
	CON_Out(console, "echo");
	CON_Out(console, "alpha");
	CON_Out(console, "background");
	CON_Out(console, "move");
	CON_Out(console, "resize");
	CON_Out(console, "prompt");
	CON_Out(console, "drawtextdemo");
}
