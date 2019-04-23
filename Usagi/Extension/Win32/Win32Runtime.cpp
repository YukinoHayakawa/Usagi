#include "Win32Runtime.hpp"

#pragma warning(push)
#pragma warning(disable:4091)
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#pragma warning(pop)

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Extension/Vulkan/VulkanGpuDevice.hpp>
#include <Usagi/Extension/Win32/Input/Win32InputManager.hpp>
#include <Usagi/Extension/Win32/Win32Helper.hpp>
#include <Usagi/Extension/Win32/Window/Win32WindowManager.hpp>
#include <Usagi/Utility/Unicode.hpp>

std::shared_ptr<usagi::Runtime> usagi::Runtime::create()
{
    return std::make_shared<Win32Runtime>();
}

LONG usagi::Win32Runtime::exceptionHandler(PEXCEPTION_POINTERS p_exception_info)
{
    instance()->writeMiniDump(p_exception_info);
    return EXCEPTION_CONTINUE_SEARCH;
}

// Modified from
// http://www.debuginfo.com/examples/effmdmpexamples.html
void usagi::Win32Runtime::writeMiniDump(
    PEXCEPTION_POINTERS p_exception_info) const
{
    // Open the file
    const auto h_file = CreateFileW(utf8To16(mMinidumpPath).c_str(),
        GENERIC_READ | GENERIC_WRITE, 0,
        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if((h_file != nullptr) && (h_file != INVALID_HANDLE_VALUE))
    {
        // Create the minidump
        // bug: cannot obtain the stack trace upon crash

        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = p_exception_info;
        mdei.ClientPointers = FALSE;

        const auto mdt = static_cast<MINIDUMP_TYPE>(
			MiniDumpWithIndirectlyReferencedMemory |
			MiniDumpScanMemory |
            MiniDumpWithDataSegs |
            MiniDumpWithHandleData |
            MiniDumpWithFullMemoryInfo |
            MiniDumpWithThreadInfo |
            MiniDumpWithUnloadedModules);

        const auto rv = MiniDumpWriteDump(
            GetCurrentProcess(), GetCurrentProcessId(),
            h_file, mdt,
            p_exception_info != nullptr ? &mdei : nullptr, nullptr, nullptr);

        if(!rv)
            LOG(error, "MiniDumpWriteDump failed: {}",
                win32::getErrorMessage(GetLastError()));
        else
            LOG(info, "Minidump created: {}", mMinidumpPath);

        // Close the file
        CloseHandle(h_file);
    }
    else
    {
        LOG(error, "CreateFile failed: {}",
            win32::getErrorMessage(GetLastError()));
    }
}

usagi::Win32Runtime::Win32Runtime()
{
    win32::patchConsole();
}

usagi::Win32Runtime::~Win32Runtime()
{
    // unique_ptrs deleted here
}

void usagi::Win32Runtime::initGpu()
{
    if(mGpu) return;
    mGpu = std::make_unique<VulkanGpuDevice>();
}

void usagi::Win32Runtime::initInput()
{
    if(mInputManager) return;
    mInputManager = std::make_unique<Win32InputManager>();
}

void usagi::Win32Runtime::initWindow()
{
    if(mWindowManager) return;
    mWindowManager = std::make_unique<Win32WindowManager>();
}

void usagi::Win32Runtime::enableCrashHandler(
    const std::string &report_file_path)
{
    mMinidumpPath = report_file_path;
    mMinidumpPath += ".dmp";
    SetUnhandledExceptionFilter(&exceptionHandler);
}

void usagi::Win32Runtime::displayErrorDialog(const std::string &msg)
{
    LOG(error, msg);
    MessageBoxW(nullptr, utf8To16(msg).c_str(), L"Error", MB_OK | MB_ICONERROR);
}

usagi::GpuDevice * usagi::Win32Runtime::gpu() const
{
    return mGpu.get();
}

usagi::InputManager * usagi::Win32Runtime::inputManager() const
{
    return mInputManager.get();
}

usagi::WindowManager * usagi::Win32Runtime::windowManager() const
{
    return mWindowManager.get();
}
