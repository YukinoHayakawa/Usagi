#pragma once

#include <string>

#include "Win32.hpp"

namespace usagi::win32
{
std::string getErrorMessage(DWORD error_code);

/**
 * \brief Enable the Win32 console to perform I/O with UTF-8 strings.
 */
void patchConsole();
}
