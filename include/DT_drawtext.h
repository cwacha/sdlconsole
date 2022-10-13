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

#ifndef Drawtext_h
#define Drawtext_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SDL.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct BitFont_td
	{
		SDL_Surface *FontSurface;
		unsigned int CharWidth;
		unsigned int CharHeight;
		unsigned int Cols;
		unsigned int Rows;
		Uint32 map[256];
	} BitFont;

	/// @brief Return the pixel value at (x, y). If x or y are larger than the surface returns the most bottom right pixel
	/// @param surface The SDL_Surface to get the pixel from
	/// @param x x-coordinate for the pixel
	/// @param y y-coordinate for the pixel
	/// @return the pixel value
	Uint32 BF_GetPixel(SDL_Surface *surface, unsigned int x, unsigned int y);

	/// @brief Opens a bitmap font from a supported file (BMP format, or any other if SDL_image is used)
	/// @param BitmapFilename Filename to a font image
	/// @param format SDL_PixelFormat of the output screen SDL_Surface to ensure the stored picture works well with the target surface
	/// @return BitFont struct holding the font data, NULL if an error occurred
	BitFont *BF_OpenFont(const char *BitmapFilename, SDL_PixelFormat *format);

	/// @brief Renders text to the surface using the specified font.
	/// @param font the font to use
	/// @param text the text you want to draw
	/// @param surface destination surface to draw to
	/// @param x coordinate on the destination surface where text should be drawn
	/// @param y coordinate on the destination surface where text should be drawn
	void BF_RenderText(BitFont *font, const char *text, SDL_Surface *surface, int x, int y);

	/// @brief Close and cleanup loaded font once it is no longer required.
	/// @param font The BitFont to close
	void BF_CloseFont(BitFont *font);

	void BF_LoadMap(BitFont *font, const char *utf8_map);
	int BF_GetMapIndex(BitFont *font, Uint32 codepoint);

#ifdef __cplusplus
};
#endif

#endif
