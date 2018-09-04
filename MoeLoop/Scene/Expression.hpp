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
    Vector2f mOrigin = Vector2f::Zero();
    Vector2f mFaceCenter = Vector2f::Zero();
    // todo animation sequence
    std::shared_ptr<GpuImage> mTexture;

public:
    Expression(
        Element *parent,
        std::string name,
        Vector2f origin,
        Vector2f face_center,
        std::shared_ptr<GpuImage> gpu_image);

    /**
     * \brief Relative to the top-left corner of the texture.
     * \return
     */
    Vector2f origin() const { return mOrigin; }

    std::shared_ptr<GpuImage> texture() const;
    AlignedBox2f textureUvRect() const;
};
}
