#pragma once

#include <stdexcept>
// todo: remove win32 header
#include <windows.h>

#define YUKI_DECL_RUNTIME_EXCEPTION(exception_name) \
class exception_name ## Exception : public std::runtime_error { \
public: \
    using std::runtime_error::runtime_error; \
    DWORD getWin32ErrorCode() { return mWin32ErrorCode; } \
protected: \
    DWORD mWin32ErrorCode = GetLastError(); \
} \
/**/
