#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuDevice;
class InputManager;
class WindowManager;

/**
 * \brief Provides access to platform-dependent resources including GPU,
 * input system, and windowing system.
 */
class Runtime final : Noncopyable
{
     std::unique_ptr<GpuDevice> mGpu;
     std::unique_ptr<InputManager> mInputManager;
     std::unique_ptr<WindowManager> mWindowManager;

public:
    // Implement the init methods for each specific platform.

    Runtime();
    ~Runtime();

    void initGpu();
    void initInput();
    void initWindow();

    GpuDevice * gpu() const
    {
        return mGpu.get();
    }

    InputManager * inputManager() const
    {
        return mInputManager.get();
    }

    WindowManager * windowManager() const
    {
        return mWindowManager.get();
    }
};
}
