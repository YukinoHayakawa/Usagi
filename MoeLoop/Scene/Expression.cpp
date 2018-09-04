#include "Expression.hpp"

namespace usagi::moeloop
{
Expression::Expression(
    Element *parent,
    std::string name,
    Vector2f origin,
    Vector2f face_center,
    std::shared_ptr<GpuImage> gpu_image)
    : Element(parent, std::move(name))
    , mOrigin(std::move(origin))
    , mFaceCenter(std::move(face_center))
    , mTexture(std::move(gpu_image))
{
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
