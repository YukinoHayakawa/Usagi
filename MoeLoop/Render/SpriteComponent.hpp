#pragma once

#include <Usagi/Core/Component.hpp>
#include <Usagi/Core/Math.hpp>

namespace usagi
{
class GpuImage;
}

namespace usagi::moeloop
{
struct SpriteComponent : Component
{
    bool show = true;
    std::shared_ptr<GpuImage> texture;
    AlignedBox2f uv_rect = { Vector2f::Zero(), Vector2f::Ones() };
    float fade = 1.f;

    const std::type_info & baseType() override
    {
        return typeid(SpriteComponent);
    }
};
}
