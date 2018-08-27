#pragma once

#include <map>

#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Utility/Singleton.hpp>

#include "Win32RawInputDevice.hpp"

namespace usagi
{
class Win32RawInputDevice;
class Win32Keyboard;
class Win32Mouse;

class Win32InputManager
    : Singleton<Win32InputManager>
    , public InputManager
{
    static inline const wchar_t WINDOW_CLASS_NAME[] = L"UsagiInputSink";
    HINSTANCE mProcessInstanceHandle = GetModuleHandleW(nullptr);
    HWND mMessageWindow = nullptr;

    void registerWindowClass() const;
    void unregisterWindowClass() const;
    void createInputSinkWindow();

    static LRESULT CALLBACK inputMessageDispatcher(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK inputMessageHandler(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void registerRawInputDevices();

    // maps device object name to friendly name or driver desc for the user
    // to identify the devices.
    // the device object names are obtained by querying the symbolic link
    // target of device path.
    // strings are stored in UTF-8 encoding.
    // <device object name, friendly name>
    static std::map<std::string, std::string> mDeviceNames;

    static void updateDeviceNames();

    Win32RawInputDeviceEnumeration mDeviceEnumeration;

    std::shared_ptr<Win32Keyboard> mVirtualKeyboard;
    std::shared_ptr<Win32Mouse> mVirtualMouse;

    void createVirtualDevices();

    std::map<HANDLE, std::weak_ptr<Win32RawInputDevice>> mRawInputDevices;
    std::vector<BYTE> mRawInputBuffer;

    void fillRawInputBuffer(LPARAM lParam);

public:
    Win32InputManager();
    ~Win32InputManager();

    void processEvents() override;

    std::shared_ptr<Keyboard> virtualKeyboard() const override;
    std::shared_ptr<Mouse> virtualMouse() const override;
    std::vector<std::shared_ptr<Gamepad>> gamepads() const override;

    // Win32

    static std::string deviceFriendlyName(
        const std::string &device_object_name);
};
}
