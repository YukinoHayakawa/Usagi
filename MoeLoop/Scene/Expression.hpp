#pragma once

#include <string>

#include <Usagi/Core/Math.hpp>
#include <Usagi/Core/Element.hpp>

namespace usagi
{
class GpuImage;
}

namespace usagi::moeloop
{
class Expression : public Element
{
    Vector2f mOrigin;
    std::shared_ptr<GpuImage> mTexture;

public:
    Expression(
        Element *parent,
        std::string name,
        std::shared_ptr<GpuImage> texture);

    Vector2f origin() const { return mOrigin; }
    std::shared_ptr<GpuImage> texture() const;
    AlignedBox2f textureUvRect() const;
};
}
