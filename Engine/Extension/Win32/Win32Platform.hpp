#pragma once

#include <map>

#include <Usagi/Engine/Runtime/Platform.hpp>

#include "Win32.hpp"

namespace usagi
{
class Win32RawInputDevice;
class Win32Keyboard;
class Win32Mouse;
class Win32Window;

/**
 * \brief Talking with Win32 messaging mechanism and discover devices.
 * One application can have at most one instance.
 */
class Win32Platform : public Platform
{
    HINSTANCE mProcessInstanceHandle = nullptr;

    void registerWindowClass();
    void unregisterWindowClass();
    static void registerRawInputDevices();

    /**
     * \brief Get the window pointer from a Win32 window handle. If the window
     * is not created by this class, a nullptr is returned.
     * \return 
     */
    static Win32Window * windowFromHandle(HWND handle);

    std::shared_ptr<Win32Keyboard> mVirtualKeyboard;
    std::shared_ptr<Win32Mouse> mVirtualMouse;

    void createVirtualDevices();

    std::map<HANDLE, std::weak_ptr<Win32RawInputDevice>> mRawInputDevices;

    std::vector<BYTE> mRawInputBuffer;
    void fillRawInputBuffer(LPARAM lParam);

    static Win32Platform *mInstance;

    LRESULT CALLBACK handleWindowMessage(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    // maps device object name to friendly name or driver desc for the user
    // to identify the devices.
    // the device object names are obtained by querying the symbolic link
    // target of device path.
    static std::map<std::wstring, std::string> mDeviceNames;


public:
    Win32Platform();
    ~Win32Platform();
    static void updateDeviceNames();

    std::shared_ptr<Window> createWindow(
        const std::string &title,
        const Vector2i &position,
        const Vector2u32 &size) override;

    void processEvents() override;

    std::shared_ptr<Keyboard> virtualKeyboard() const override;
    std::shared_ptr<Mouse> virtualMouse() const override;
    const std::vector<std::shared_ptr<Gamepad>> & gamepads() const override;

    // Win32

    static const wchar_t WINDOW_CLASS_NAME[];

    static LRESULT CALLBACK windowMessageDispatcher(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    HINSTANCE processInstanceHandle() const { return mProcessInstanceHandle; }

    /**
     * \brief Get the window having input focus. If the foreground window
     * is not owned by our process, nullptr is returned.
     * \return 
     */
    static Win32Window * getActiveWindow();
};
}
