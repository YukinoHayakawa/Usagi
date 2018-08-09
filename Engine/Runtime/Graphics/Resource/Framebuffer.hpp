#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
class Framebuffer : Noncopyable
{
public:
    virtual ~Framebuffer() = default;

    virtual Vector2u32 size() const = 0;
};
}
