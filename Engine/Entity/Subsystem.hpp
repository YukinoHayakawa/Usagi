#pragma once

#include <chrono>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{
class Subsystem : Noncopyable
{
public:
    virtual ~Subsystem() = default;

    virtual void update(const std::chrono::milliseconds &dt) = 0;
};
}
