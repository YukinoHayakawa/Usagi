#pragma once

#include <string>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

#include "Win32.hpp"

namespace usagi
{
class Win32Window;

class Win32RawInputDevice : Noncopyable
{
protected:
    HANDLE mDeviceHandle = nullptr;
    std::string mName;

    void checkDevice(RAWINPUT *raw, DWORD type) const;

public:
    Win32RawInputDevice(HANDLE device_handle, std::string name);
    virtual ~Win32RawInputDevice() = default;

    virtual void handleRawInput(RAWINPUT *data) = 0;
};
}
