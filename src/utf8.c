#include "utf8.h"
#include "errno.h"

int u8chrisvalid(u8chr_t c)
{
    if (c <= 0x7F)
        return 1; // [1]

    if (0xC280 <= c && c <= 0xDFBF) // [2]
        return ((c & 0xE0C0) == 0xC080);

    if (0xEDA080 <= c && c <= 0xEDBFBF) // [3]
        return 0;                       // Reject UTF-16 surrogates

    if (0xE0A080 <= c && c <= 0xEFBFBF) // [4]
        return ((c & 0xF0C0C0) == 0xE08080);

    if (0xF0908080 <= c && c <= 0xF48FBFBF) // [5]
        return ((c & 0xF8C0C0C0) == 0xF0808080);

    return 0;
}

int u8next(const char *txt, u8chr_t *ch)
{
    int len;
    u8chr_t encoding = 0;

    len = u8length(txt);

    for (int i = 0; i < len && txt[i] != '\0'; i++)
    {
        encoding = (encoding << 8) | (uint8_t)txt[i];
    }

    errno = 0;
    if (len == 0 || !u8chrisvalid(encoding))
    {
        encoding = txt[0];
        len = 1;
        errno = EINVAL;
    }

    if (ch)
        *ch = encoding;

    return encoding ? len : 0;
}

// from UTF-8 encoding to Unicode Codepoint
uint32_t u8decode(u8chr_t c)
{
    uint32_t mask;

    if (c > 0x7F)
    {
        mask = (c <= 0x00EFBFBF) ? 0x000F0000 : 0x003F0000;
        c = ((c & 0x07000000) >> 6) |
            ((c & mask) >> 4) |
            ((c & 0x00003F00) >> 2) |
            (c & 0x0000003F);
    }

    return c;
}

// From Unicode Codepoint to UTF-8 encoding
u8chr_t u8encode(uint32_t codepoint)
{
    u8chr_t c = codepoint;

    if (codepoint > 0x7F)
    {
        c = (codepoint & 0x000003F) | (codepoint & 0x0000FC0) << 2 | (codepoint & 0x003F000) << 4 | (codepoint & 0x01C0000) << 6;

        if (codepoint < 0x0000800)
            c |= 0x0000C080;
        else if (codepoint < 0x0010000)
            c |= 0x00E08080;
        else
            c |= 0xF0808080;
    }
    return c;
}
