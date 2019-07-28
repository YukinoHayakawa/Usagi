#pragma once

#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
class Framebuffer : Noncopyable
{
public:
    virtual ~Framebuffer() = default;

    virtual Vector2u32 size() const = 0;
};
}
