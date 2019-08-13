#include "Win32Helper.hpp"

#include <iostream>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Utility/Unicode.hpp>
#include <Usagi/Utility/RawResource.hpp>

#pragma warning(disable: 4005) // macro redefinition

#include "Win32.hpp"
#define DEVICE_TYPE DWORD
typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;
#include "ntos.h"

// https://docs.microsoft.com/en-us/windows/desktop/Debug/retrieving-the-last-error-code
std::string usagi::win32::getErrorMessage(const DWORD error_code)
{
    // Retrieve the system error message for the given error code
    WCHAR *msg_buf = nullptr;

    const auto num_char = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&msg_buf),
        0,
        nullptr
    );

    if(num_char == 0)
    {
        LOG(error, "FormatMessage() failed: {}", GetLastError());
    }

    const std::wstring msg { msg_buf, num_char };
    LocalFree(msg_buf);

    return utf16To8(msg);
}

// http://blogs.microsoft.co.il/pavely/2014/02/05/creating-a-winobj-like-tool/
std::wstring usagi::win32::resolveNtSymbolicLink(const std::wstring &link)
{
    // Specify object path
    UNICODE_STRING      object_name;
    OBJECT_ATTRIBUTES   object_attributes;
    // this function does not allocate any memory
    // http://www.osronline.com/ShowThread.cfm?link=197211
    RtlInitUnicodeString(&object_name, link.c_str());
    InitializeObjectAttributes(
        &object_attributes, &object_name, OBJ_CASE_INSENSITIVE, nullptr, nullptr
    );

    // Open object
    RawResource link_handle {
        HANDLE { nullptr },
        [&](HANDLE &h) {
            const auto status = NtOpenSymbolicLinkObject(
                &h, SYMBOLIC_LINK_QUERY, &object_attributes
            );
            if(!NT_SUCCESS(status) || (h == nullptr))
            {
                throw std::runtime_error("NtOpenSymbolicLinkObject() failed.");
            }
        },
        [&](HANDLE &h) {
            NtClose(h);
        }
    };

    // Query link target
    WCHAR           target_buffer[256] = { 0 };
    UNICODE_STRING  target;
    RtlInitUnicodeString(&target, target_buffer);
    target.MaximumLength = sizeof(target_buffer);

    const auto status = NtQuerySymbolicLinkObject(
        link_handle, &target, nullptr);
    if(!NT_SUCCESS(status) || (link_handle == nullptr)) {
        throw std::runtime_error("NtQuerySymbolicLinkObject() failed.");
    }

    return { target_buffer };
}

// patch the console to allow UTF-8 I/O
// from https://stackoverflow.com/questions/1660492/utf-8-output-on-windows-console
namespace
{
class ConsoleStreamBufWin32 : public std::streambuf
{
public:
    ConsoleStreamBufWin32(DWORD handle_id, bool is_input);

protected:
    // std::basic_streambuf
    std::streambuf * setbuf(char_type *s, std::streamsize n) override;
    int sync() override;
    int_type underflow() override;
    int_type overflow(int_type c) override;

private:
    HANDLE const mHandle; // NOLINT(misc-misplaced-const)
    bool const mIsInput;
    std::string mBuffer;
};

ConsoleStreamBufWin32::ConsoleStreamBufWin32(
    const DWORD handle_id,
    const bool is_input
)   : mHandle(GetStdHandle(handle_id))
    , mIsInput(is_input)
{
    if(mIsInput) { setg(nullptr, nullptr, nullptr); }
}

std::streambuf * ConsoleStreamBufWin32::setbuf(
    char_type * /*s*/,
    std::streamsize /*n*/) { return nullptr; }

int ConsoleStreamBufWin32::sync()
{
    if(mIsInput)
    {
        FlushConsoleInputBuffer(mHandle);
        setg(nullptr, nullptr, nullptr);
    }
    else
    {
        if(mBuffer.empty()) { return 0; }

        const auto wide_buffer = usagi::utf8To16(mBuffer);
        DWORD written_size;
        WriteConsoleW(mHandle, wide_buffer.c_str(),
            static_cast<DWORD>(wide_buffer.size()), &written_size, nullptr);
    }

    mBuffer.clear();

    return 0;
}

ConsoleStreamBufWin32::int_type ConsoleStreamBufWin32::underflow()
{
    if(!mIsInput) { return traits_type::eof(); }

    if(gptr() >= egptr())
    {
        wchar_t wide_buffer[128];
        DWORD read_size;
        if(!ReadConsoleW(mHandle, wide_buffer, ARRAYSIZE(wide_buffer) - 1,
            &read_size, nullptr)) { return traits_type::eof(); }

        wide_buffer[read_size] = L'\0';
        mBuffer = usagi::utf16To8(wide_buffer);

        setg(&mBuffer[0], &mBuffer[0], &mBuffer[0] + mBuffer.size());

        if(gptr() >= egptr()) { return traits_type::eof(); }
    }

    return sgetc();
}

ConsoleStreamBufWin32::int_type ConsoleStreamBufWin32::overflow(int_type c)
{
    if(mIsInput) { return traits_type::eof(); }

    mBuffer += traits_type::to_char_type(c);
    return traits_type::not_eof(c);
}

template<typename StreamT>
void fixStdStream(const DWORD handle_id, const bool is_input, StreamT& stream)
{
    if(GetFileType(GetStdHandle(handle_id)) == FILE_TYPE_CHAR)
    {
        // todo: possible memory leak
        stream.rdbuf(new ConsoleStreamBufWin32(handle_id, is_input));
    }
}
}

void usagi::win32::patchConsole()
{
    static bool patched = false;
    if(patched) return;

    fixStdStream(STD_INPUT_HANDLE, true, std::cin);
    fixStdStream(STD_OUTPUT_HANDLE, false, std::cout);
    fixStdStream(STD_ERROR_HANDLE, false, std::cerr);

    patched = true;
}
