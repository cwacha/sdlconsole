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

/*  DT_drawtext.c
 *  Written By: Garrett Banuk <mongoose@mongeese.org>
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DT_drawtext.h"
#include "utf8.h"
#include "SDL.h"

#ifdef HAVE_SDLIMAGE
#include "SDL_image.h"
#endif

Uint32 BF_GetPixel(SDL_Surface *surface, unsigned int x, unsigned int y)
{
	int bpp = surface->format->BytesPerPixel;
	if (x > surface->w - 1)
		x = surface->w - 1;
	if (y > surface->h - 1)
		y = surface->h - 1;

	// Here p is the address to the pixel we want to retrieve
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;
	case 2:
		return *(Uint16 *)p;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;
	case 4:
		return *(Uint32 *)p;
		break;
	default:
		return 0;
	}
}

void BF_LoadMap(BitFont *font, const char *utf8_map)
{
	if (!font)
		return;

	int maplength = strlen(utf8_map);

	font->map[0] = 0;

	int i = 1;
	int character = 1;
	u8chr_t ch;
	while (i < maplength && character < 256)
	{
		i += u8next(&utf8_map[i], &ch);
		font->map[character] = u8decode(ch);
		character++;
	}
}

int BF_GetMapIndex(BitFont *font, Uint32 codepoint)
{
	if (!font)
		return 0;

	for (int i = 0; i < sizeof(font->map); i++)
	{
		if (font->map[i] == codepoint)
			return i;
	}
	return 0;
}

BitFont *
BF_OpenFont(const char *filename, SDL_PixelFormat *format)
{
	BitFont *NewFont;
	SDL_Surface *Temp;

	/* load the font bitmap */

#ifdef HAVE_SDLIMAGE
	Temp = IMG_Load(filename);
#else
	Temp = SDL_LoadBMP(filename);
#endif

	if (!Temp)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot load font file %s: %s", filename, SDL_GetError());
		return NULL;
	}

	/* Add a font to the list */
	NewFont = (BitFont *)malloc(sizeof(BitFont));

	NewFont->FontSurface = SDL_ConvertSurface(Temp, format, 0);
	SDL_FreeSurface(Temp);
	if (!NewFont->FontSurface)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to convert font surface %s: %s", filename, SDL_GetError());
		return NULL;
	}

	NewFont->CharWidth = NewFont->FontSurface->w / 32;
	NewFont->CharHeight = NewFont->FontSurface->h / 8;

	/* Set font transparency. The assumption we'll go on is that the first pixel of the font image
	 *  will be the color we should treat as transparent.
	 */
	Uint8 r, g, b;
	SDL_GetRGB(BF_GetPixel(NewFont->FontSurface, 0, 0), NewFont->FontSurface->format, &r, &g, &b);
	SDL_SetColorKey(NewFont->FontSurface, SDL_TRUE, SDL_MapRGB(NewFont->FontSurface->format, r, g, b));

	// load the default UTF8 translation map (for CP437)
	const char *BitFontDefaultMap = " ☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~⌂ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■";
	BF_LoadMap(NewFont, BitFontDefaultMap);

	return NewFont;
}

void BF_RenderText(BitFont *font, const char *text, SDL_Surface *surface, int x, int y)
{
	SDL_Rect SourceRect, DestRect;

	if (!font)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "No font provided, cannot render text.");
		return;
	}

	// don't draw if the area is outside of the surface. That was easy!
	if (x > surface->w || y > surface->h)
		return;

	/*
		int characters;
		if (strlen(text) < (surface->w - x) / font->CharWidth)
			characters = strlen(text);
		else
			characters = (surface->w - x) / font->CharWidth;
	*/
	DestRect.x = x;
	DestRect.y = y;
	DestRect.w = font->CharWidth;
	DestRect.h = font->CharHeight;

	SourceRect.y = 0;
	SourceRect.w = font->CharWidth;
	SourceRect.h = font->CharHeight;

	/* see how many characters can fit on the screen */
	int max_characters = (surface->w - x) / font->CharWidth;

	/* Now draw it */
	int text_length = strlen(text);

	int loop = 0;
	int character = 0;
	while (loop < text_length && character < max_characters)
	{
		u8chr_t ch;
		loop += u8next(&text[loop], &ch);
		int current = BF_GetMapIndex(font, u8decode(ch));
		character++;

		SourceRect.x = (current % 32) * font->CharWidth;
		SourceRect.y = (current / 32) * font->CharHeight;

		SDL_BlitSurface(font->FontSurface, &SourceRect, surface, &DestRect);
		DestRect.x += font->CharWidth;
	}

	/*
		int loop;
		int current;
		for (loop = 0; loop < characters; loop++)
		{
			current = text[loop];
			if (current < 0 || current > 255)
			{
				// SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unknown character code: %i", current);
				current = 0;
			}
			// SourceRect.x = string[loop] * CurrentFont->CharWidth;
			SourceRect.x = (current % 32) * font->CharWidth;
			SourceRect.y = (current / 32) * font->CharHeight;

			SDL_BlitSurface(font->FontSurface, &SourceRect, surface, &DestRect);
			DestRect.x += font->CharWidth;
		}
	*/
}
void BF_CloseFont(BitFont *font)
{
	if (!font)
		return;
	SDL_FreeSurface(font->FontSurface);
	free(font);
}
