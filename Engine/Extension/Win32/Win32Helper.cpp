#include "Win32Helper.hpp"

#include <iostream>

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Utility/String.hpp>

// https://docs.microsoft.com/en-us/windows/desktop/Debug/retrieving-the-last-error-code
std::string usagi::win32::getErrorMessage(DWORD error_code)
{
    // Retrieve the system error message for the given error code
    WCHAR *lpMsgBuf = nullptr;

    const auto num_char = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&lpMsgBuf),
        0,
        nullptr
    );

    if(num_char == 0)
    {
        LOG(warn, "FormatMessage() failed: {}", GetLastError());
    }

    const std::wstring msg { lpMsgBuf, num_char };
    LocalFree(lpMsgBuf);

    return ws2s(msg);
}

std::wstring usagi::win32::resolveNtSymbolicLink(const std::wstring &link)
{
    return { };
}

// patch the console to allow UTF-8 I/O
// from https://stackoverflow.com/questions/1660492/utf-8-output-on-windows-console
namespace
{
class ConsoleStreamBufWin32 : public std::streambuf
{
public:
    ConsoleStreamBufWin32(DWORD handleId, bool isInput);

protected:
    // std::basic_streambuf
    std::streambuf * setbuf(char_type *s, std::streamsize n) override;
    int sync() override;
    int_type underflow() override;
    int_type overflow(int_type c) override;

private:
    HANDLE const m_handle;
    bool const m_isInput;
    std::string m_buffer;
};

ConsoleStreamBufWin32::ConsoleStreamBufWin32(DWORD handleId, bool isInput)
    : m_handle(::GetStdHandle(handleId))
    , m_isInput(isInput)
{
    if(m_isInput) { setg(0, 0, 0); }
}

std::streambuf * ConsoleStreamBufWin32::setbuf(
    char_type * /*s*/,
    std::streamsize /*n*/) { return nullptr; }

int ConsoleStreamBufWin32::sync()
{
    if(m_isInput)
    {
        ::FlushConsoleInputBuffer(m_handle);
        setg(0, 0, 0);
    }
    else
    {
        if(m_buffer.empty()) { return 0; }

        const auto wide_buffer = usagi::s2ws(m_buffer);
        DWORD writtenSize;
        ::WriteConsoleW(m_handle, wide_buffer.c_str(),
            static_cast<DWORD>(wide_buffer.size()), &writtenSize, nullptr);
    }

    m_buffer.clear();

    return 0;
}

ConsoleStreamBufWin32::int_type ConsoleStreamBufWin32::underflow()
{
    if(!m_isInput) { return traits_type::eof(); }

    if(gptr() >= egptr())
    {
        wchar_t wide_buffer[128];
        DWORD read_size;
        if(!::ReadConsoleW(m_handle, wide_buffer, ARRAYSIZE(wide_buffer) - 1,
            &read_size, nullptr)) { return traits_type::eof(); }

        wide_buffer[read_size] = L'\0';
        m_buffer = usagi::ws2s(wide_buffer);

        setg(&m_buffer[0], &m_buffer[0], &m_buffer[0] + m_buffer.size());

        if(gptr() >= egptr()) { return traits_type::eof(); }
    }

    return sgetc();
}

ConsoleStreamBufWin32::int_type ConsoleStreamBufWin32::overflow(int_type c)
{
    if(m_isInput) { return traits_type::eof(); }

    m_buffer += traits_type::to_char_type(c);
    return traits_type::not_eof(c);
}

template<typename StreamT>
void fixStdStream(DWORD handleId, bool isInput, StreamT& stream)
{
    if(GetFileType(GetStdHandle(handleId)) == FILE_TYPE_CHAR)
    {
        stream.rdbuf(new ConsoleStreamBufWin32(handleId, isInput));
    }
}
}

void usagi::win32::patchConsole()
{
    fixStdStream(STD_INPUT_HANDLE, true, std::cin);
    fixStdStream(STD_OUTPUT_HANDLE, false, std::cout);
    fixStdStream(STD_ERROR_HANDLE, false, std::cerr);
}
