#include "Expression.hpp"

namespace usagi::moeloop
{
Expression::Expression(
    Element *parent,
    std::string name,
    std::shared_ptr<GpuImage> texture)
    : Element(parent, std::move(name))
{
    mTexture = std::move(texture);
}

std::shared_ptr<GpuImage> Expression::texture() const
{
    return mTexture;
}

AlignedBox2f Expression::textureUvRect() const
{
    return { Vector2f::Zero(), Vector2f::Ones() };
}
}
