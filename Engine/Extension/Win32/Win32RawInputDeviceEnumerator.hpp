#pragma once

#include "Win32.hpp"

namespace usagi
{
class Win32RawInputDeviceEnumerator
{
public:
    static void enumerateDevices();
};
}
