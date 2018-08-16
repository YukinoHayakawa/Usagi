#pragma once

#include <string>

namespace usagi
{
std::wstring u8to16(const std::string &string);
std::string u16to8(const std::wstring &string);

// conversion code from
// https://blog.keyman.com/2011/06/accepting-unicode-input-in-your-windows-application/
constexpr bool isUtf16HighSurrogate(const wchar_t ch)
{
    return ch >= 0xD800 && ch <= 0xDBFF;
}

constexpr bool isUtf16LowSurrogate(const wchar_t ch)
{
    return ch >= 0xDC00 && ch <= 0xDFFF;
}

constexpr wchar_t utf16SurrogatesToUtf32(const wchar_t ch, const wchar_t cl)
{
    return (ch - 0xD800) * 0x400 + (cl - 0xDC00) + 0x10000;
}

// https://stackoverflow.com/questions/42012563/convert-unicode-code-points-to-utf-8-and-utf-32
constexpr std::uint8_t codePointToUtf8(
    char *const buffer, const std::uint32_t code)
{
    if(code <= 0x7F)
    {
        buffer[0] = code;
        return 1;
    }
    if(code <= 0x7FF)
    {
        buffer[0] = 0xC0 | (code >> 6);            /* 110xxxxx */
        buffer[1] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 2;
    }
    if(code <= 0xFFFF)
    {
        buffer[0] = 0xE0 | (code >> 12);           /* 1110xxxx */
        buffer[1] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
        buffer[2] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 3;
    }
    if(code <= 0x10FFFF)
    {
        buffer[0] = 0xF0 | (code >> 18);           /* 11110xxx */
        buffer[1] = 0x80 | ((code >> 12) & 0x3F);  /* 10xxxxxx */
        buffer[2] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
        buffer[3] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
        return 4;
    }
    return 0;
}
}
