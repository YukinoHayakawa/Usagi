#pragma once

#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Core/Math.hpp>

namespace usagi
{
class Framebuffer : Noncopyable
{
public:
    virtual ~Framebuffer() = default;

    virtual Vector2u32 size() const = 0;
};
}
