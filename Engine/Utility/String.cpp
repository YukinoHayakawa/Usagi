#include "String.hpp"

#include <string>

#include <utfcpp/utf8.h>

std::wstring usagi::s2ws(const std::string &string)
{
    std::wstring utf16line;
    utf8::utf8to16(string.begin(), string.end(),
        back_inserter(utf16line));
    return utf16line;
}

std::string usagi::ws2s(const std::wstring &string)
{
    std::string utf8line;
    utf8::utf16to8(string.begin(), string.end(), back_inserter(utf8line));
    return utf8line;
}
