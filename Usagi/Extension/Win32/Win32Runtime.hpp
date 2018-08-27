#pragma once

#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Utility/Singleton.hpp>

#include "Win32.hpp"
#include "Win32MacroFix.hpp"

namespace usagi
{
class VulkanGpuDevice;
class Win32InputManager;
class Win32WindowManager;

class Win32Runtime
    : Singleton<Win32Runtime>
    , public Runtime
{
    std::unique_ptr<VulkanGpuDevice> mGpu;
    std::unique_ptr<Win32InputManager> mInputManager;
    std::unique_ptr<Win32WindowManager> mWindowManager;
    std::string mMinidumpPath;

    static LONG WINAPI exceptionHandler(PEXCEPTION_POINTERS p_exception_info);
    void writeMiniDump(PEXCEPTION_POINTERS p_exception_info) const;

public:
    Win32Runtime();
    virtual ~Win32Runtime();

    void initGpu() override;
    void initInput() override;
    void initWindow() override;

    void enableCrashHandler(const std::string &report_file_path) override;
    void displayErrorDialog(const std::string &msg) override;

    GpuDevice * gpu() const override;
    InputManager * inputManager() const override;
    WindowManager * windowManager() const override;
};
}
