#pragma once

#include <string>

#include "Win32.hpp"

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
    DWORD mErrorCode = GetLastError();
    std::string mMessage = getErrorMessage(mErrorCode);

public:
    explicit Win32Exception(const std::string &_Message)
        : runtime_error { _Message }
    {
    }

    explicit Win32Exception(const char *_Message)
        : runtime_error { _Message }
    {
    }
};
}
