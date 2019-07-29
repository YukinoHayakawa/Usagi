#include "Unicode.hpp"

#include <string>

#include <utf8.h>

std::wstring usagi::utf8To16(std::string_view string)
{
    std::wstring utf16line;
    utf8::utf8to16(string.begin(), string.end(),
        back_inserter(utf16line));
    return utf16line;
}

std::string usagi::utf16To8(std::wstring_view string)
{
    std::string utf8line;
    utf8::utf16to8(string.begin(), string.end(), back_inserter(utf8line));
    return utf8line;
}

std::u32string usagi::utf8To32(std::string_view string)
{
    std::u32string utf32str;
    utf8::utf8to32(
        string.begin(), string.end(),
        std::back_inserter(utf32str)
    );
    return utf32str;
}
