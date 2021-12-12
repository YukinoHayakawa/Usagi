#pragma once

#include <string_view>

namespace usagi
{
// Return a string view with UTF-8 BOM removed if any.
// todo shouldn't be member func
std::string_view without_utf8_bom(std::string_view view);
}
