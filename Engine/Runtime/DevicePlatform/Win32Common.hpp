#pragma once

#include <stdexcept>
// todo: remove win32 header
#include <windows.h>

namespace yuki
{

class Win32APIException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;

    DWORD getErrorCode() const
    {
        return mWin32ErrorCode;
    }

protected:
    DWORD mWin32ErrorCode = GetLastError();
};

}
