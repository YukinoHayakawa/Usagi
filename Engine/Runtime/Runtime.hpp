#pragma once

#include <memory>
#include <string>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuDevice;
class InputManager;
class WindowManager;

/**
 * \brief Provides access to platform-dependent resources and other
 * functionalities.
 */
class Runtime : Noncopyable
{
public:
    virtual ~Runtime() = default;

    // Each platform should implements a factory method.
    static std::shared_ptr<Runtime> create();

    virtual void initGpu() = 0;
    virtual void initInput() = 0;
    virtual void initWindow() = 0;

    /**
     * \brief
     * \param report_file_path Without file extension.
     */
    virtual void enableCrashHandler(const std::string &report_file_path) = 0;
    virtual void displayErrorDialog(const std::string & msg) = 0;

    virtual GpuDevice * gpu() const = 0;
    virtual InputManager * inputManager() const = 0;
    virtual WindowManager * windowManager() const = 0;
};
}
