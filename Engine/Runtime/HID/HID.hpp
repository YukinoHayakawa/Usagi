#pragma once

#include <string>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class HID : Noncopyable
{
public:
    virtual ~HID() = default;

    virtual std::string name() const = 0;
};
}
