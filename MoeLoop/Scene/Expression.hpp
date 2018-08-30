#pragma once

#include <string>

#include <Usagi/Core/Math.hpp>

namespace usagi
{
class GpuImage;
}

namespace usagi::moeloop
{
class Expression
{
    Vector2f mOrigin;
    std::string mName;

public:
    Expression(std::string name)
        : mName(std::move(name))
    {
    }

    std::string name();
    Vector2f origin() const { return mOrigin; }
    std::shared_ptr<GpuImage> texture() const;
    AlignedBox2f textureUvRect() const;
};
}
