#pragma once

#include <string>

namespace usagi
{
std::string sha256(const std::string &string);
std::string crc32(const std::string &string);
}
