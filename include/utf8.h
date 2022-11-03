#ifndef _UTF8_H_
#define _UTF8_H_

#include <stdint.h>

typedef uint32_t u8chr_t;

static uint8_t const u8_length[] = {
    // 1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 3, 4};

/// @brief Returns the length in bytes of the first UTF8 character pointed to by s
/// @param s The string to look at
/// @return the length in bytes of the first character
#define u8length(s) u8_length[(((uint8_t *)(s))[0] & 0xFF) >> 4];

/// @brief Test if c is the start of a UTF8 sequence
/// @param c The character to test
/// @return 1 if c is a start sequence, 0 else
#define isutf(c) (((c)&0xC0) != 0x80)

/// @brief Tests if provided UTF8 character byte combination is a valid UTF8 character
/// @param c The UTF8 character to test
/// @return 1 if character is valid, 0 else
int u8chrisvalid(u8chr_t c);

/// @brief Returns index in bytes of n-th UTF-8 character (given by charnum)
/// @param s The string to search in
/// @param charnum The n-th UTF-8 character to find
/// @return the byte index where the n-th character starts
size_t u8offset(const char *s, size_t charnum);

/// @brief Decreases i by one UTF8 character which means moving it by 1, 2, 3 or 4 bytes to the left.
///        The i byte index must be inside the size of string s (no bounds checking)
/// @param s The string to search
/// @param i the byte position to start, i will be modified to point to the next character on the left
void u8_dec(const char *s, size_t *i);

/// @brief Increases i by one UTF8 characer which means moving it by 1, 2, 3 or 4 bytes to the right.
///        The i byte index must be inside the size of the string s (no bounds checking)
/// @param s The string to search
/// @param i the byte position to start, i will be modified to point to the next character on the right.
void u8_inc(const char *s, size_t *i);

/// @brief Parses txt and stores the next detected UTF8 character in ch. Returns length of character in bytes (1, 2, 3, 4)
///        On error return 1 and sets errno to EINVAL.
/// @param txt The string to parse
/// @param ch Pointer to next parsed UTF8 character
/// @return the number of bytes that parsed UTF8 character requires
int u8next(const char *txt, u8chr_t *ch);

/// @brief Decode UTF8 character into Unicode Codepoint
/// @param ch The character to decode
/// @return the Unicode Codepoint
uint32_t u8decode(u8chr_t ch);

/// @brief Encodes a Unicode Codepoint value into a UTF8 character
/// @param codepoint The Unicode Codepoint value to encode
/// @return The corresponding UTF8 character
u8chr_t u8encode(uint32_t codepoint);

/// @brief Returns number of Unicode Codepoints in given string which is the actual number of characters in a UTF8 string
///        Equivalent of strlen() function for UTF8 strings
/// @param s The string to measure
/// @return The number of UTF8 characters in the string
size_t u8strlen(const char *s);

#endif /* _UTF8_H_ */