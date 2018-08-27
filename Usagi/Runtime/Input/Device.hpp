#pragma once

#include <string>

#include <Usagi/Utility/Noncopyable.hpp>

#include "DeviceEventListener.hpp"

namespace usagi
{
class Device : Noncopyable
{
public:
    virtual ~Device() = default;

    virtual std::string name() const = 0;
};
}
