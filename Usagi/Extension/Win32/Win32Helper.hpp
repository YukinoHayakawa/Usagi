#pragma once

#include <string>
#include <stdexcept>

#include "Win32.hpp"
#include "Win32MacroFix.hpp"

namespace usagi::win32
{
/**
 * \brief Get UTF-8 message string of error code.
 * \param error_code
 * \return
 */
std::string getErrorMessage(DWORD error_code);
std::wstring resolveNtSymbolicLink(const std::wstring &link);

/**
 * \brief Enable the Win32 console to perform I/O with UTF-8 strings.
 */
void patchConsole();

class Win32Exception : public std::runtime_error
{
public:
    const DWORD error_code = GetLastError();
    const std::string message = getErrorMessage(error_code);

    using runtime_error::runtime_error;
};
}
