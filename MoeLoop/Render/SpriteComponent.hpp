#pragma once

#include <Usagi/Core/Component.hpp>
#include <Usagi/Core/Math.hpp>

namespace usagi
{
class GpuImage;
}

namespace usagi::moeloop
{
struct SpriteLayer
{
    std::shared_ptr<GpuImage> texture;
    Vector2f offset = Vector2f::Zero();
    AlignedBox2f uv_rect = { Vector2f::Zero(), Vector2f::Ones() };
    float factor = 0.f;

    AlignedBox2f quad() const;
    AlignedBox2f toUV(AlignedBox2f quad) const;
};

struct SpriteComponent : Component
{
    bool show = true;
    constexpr static std::size_t NUM_LAYERS = 2;
    SpriteLayer layers[NUM_LAYERS];

    const std::type_info & baseType() override final
    {
        return typeid(SpriteComponent);
    }
};
}
