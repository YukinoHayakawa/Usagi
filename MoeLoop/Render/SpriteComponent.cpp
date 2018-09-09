#include "SpriteComponent.hpp"

#include <Usagi/Runtime/Graphics/GpuImage.hpp>

usagi::AlignedBox2f usagi::moeloop::SpriteLayer::quad() const
{
    // sprite coordinate:
    // +------>x
    // |
    // |
    // *
    // y
    const auto size = uv_rect.sizes()
        .cwiseProduct(texture->size().cast<float>());
    AlignedBox2f box { Vector2f { 0, 0 }, size };
    box.translate(offset);
    return box;
}

usagi::AlignedBox2f usagi::moeloop::SpriteLayer::toUV(
    AlignedBox2f quad) const
{
    const auto tex_size = texture->size().cast<float>();
    quad.translate(-offset);
    quad.min() = quad.min().cwiseQuotient(tex_size);
    quad.max() = quad.max().cwiseQuotient(tex_size);
    return quad;
}
