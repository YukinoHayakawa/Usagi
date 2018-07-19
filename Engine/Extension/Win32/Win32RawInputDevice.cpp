#include "Win32RawInputDevice.hpp"

#include <cassert>

#include "Win32Platform.hpp"

void usagi::Win32RawInputDevice::checkDevice(RAWINPUT *raw, DWORD type) const
{
    assert(raw->header.dwType == type);
    // if mDeviceHandle is nullptr, messages from any device of the same
    // type are processed.
    if(mDeviceHandle != nullptr)
        assert(raw->header.hDevice == mDeviceHandle);
}

usagi::Win32RawInputDevice::Win32RawInputDevice(
    HANDLE device_handle,
    std::string name)
    : mDeviceHandle { device_handle }
    , mName { std::move(name) }
{
}
