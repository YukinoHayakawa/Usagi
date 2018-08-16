#pragma once

#include <string>

namespace usagi
{
std::wstring u8to16(const std::string &string);
std::string u16to8(const std::wstring &string);
}
