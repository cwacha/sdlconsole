#include <stdint.h>

typedef uint32_t u8chr_t;

static uint8_t const u8_length[] = {
    // 1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 3, 4};

#define u8length(s) u8_length[(((uint8_t *)(s))[0] & 0xFF) >> 4];

int u8chrisvalid(u8chr_t c);

int u8next(const char *txt, u8chr_t *ch);

// from UTF-8 encoding to Unicode Codepoint
uint32_t u8decode(u8chr_t c);

// From Unicode Codepoint to UTF-8 encoding
u8chr_t u8encode(uint32_t codepoint);
