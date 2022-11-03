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

#include "utf8.h"
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
		Uint32 map[256];
	} BitFont;

	/// @brief Opens a bitmap font from a supported file (BMP format, or any other if SDL_image is used).
	///        By default the picture is assumed to hold 32 characters per line in 8 rows (=256 characters).
	///        All characters must have same width and height. Width and height are auto-calculated using the 32 by 8 assumption.
	///        Other layouts are supported by calling BF_SetCharSize() after calling BF_OpenFont().
	///        The BitFont is initialized with the CP437 unicode translation. This can be changed by providing
	///        your own translation and calling BF_LoadMap().
	/// @param BitmapFilename Filename to a font image
	/// @param format SDL_PixelFormat of the output screen SDL_Surface to ensure the stored picture works well with the target surface
	/// @return BitFont struct holding the font data, NULL if an error occurred
	BitFont *BF_OpenFont(const char *BitmapFilename, SDL_PixelFormat *format);

	/// @brief Renders text to the surface using the specified font.
	/// @param font the BitFont to use
	/// @param text the text you want to draw in ASCII or UTF8 encoding
	/// @param surface destination surface to draw to
	/// @param x coordinate on the destination surface where text should be drawn
	/// @param y coordinate on the destination surface where text should be drawn
	void BF_RenderText(BitFont *font, const char *text, SDL_Surface *surface, int x, int y);

	/// @brief Close and cleanup loaded font once it is no longer required.
	/// @param font The BitFont to close
	void BF_CloseFont(BitFont *font);

	/// @brief Updates the map of supported unicode characters in the BitFont.
	///        Each BitFont supports 256 different symbols that can be freely mapped with this function
	///        The first character in the utf8_map parameter will map to the first symbol in your font etc.
	///        When a unicode characters should be rendered it will check this map what font symbol to use.
	///        If the unicode character is not listed in the map the render function will print symbol 0 instead.
	/// @param font The BitFont to update
	/// @param utf8_map A UTF8 formatted string. The first 256 characters are stored in the map.
	void BF_LoadMap(BitFont *font, const char *utf8_map);

	/// @brief Set character size and auto-calculate number of columns (and rows) in font bitmap
	/// @param font the BitFont to set the character width and height
	/// @param width the actual width of characters used in your font. Set to 0 to assume 32 characters per line and auto-calculate width based on width of the font bitmap.
	/// @param height the actual height of characters used in your font. Set to 0 to assume 8 rows of characters and auto-calculate height based on height of the font bitmap.
	void BF_SetCharSize(BitFont *font, unsigned int width, unsigned int height);

	/// @brief (internal) Translates a unicode codepoint (effective unicode number) into a map index.
	/// @param font the BitFont to use for the translation
	/// @param codepoint the unicode character number to translate
	/// @return the map location as number between 0 and 255 if the codepoint was found or 0 otherwise.
	int _BF_GetMapIndex(BitFont *font, Uint32 codepoint);

	/// @brief (internal) Return the pixel value at (x, y). If x or y are larger than the surface returns the most bottom right pixel
	/// @param surface The SDL_Surface to get the pixel from
	/// @param x x-coordinate for the pixel
	/// @param y y-coordinate for the pixel
	/// @return the pixel value (taken from SDL_Surface::pixels)
	Uint32 _BF_GetPixel(SDL_Surface *surface, unsigned int x, unsigned int y);

#ifdef __cplusplus
};
#endif

#endif
