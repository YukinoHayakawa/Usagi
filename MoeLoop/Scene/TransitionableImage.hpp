#pragma once

#include <Usagi/Core/PredefinedElement.hpp>

namespace usagi
{
struct TransformComponent;
struct AnimationComponent;
class GpuImage;
}

namespace usagi::moeloop
{
struct SpriteComponent;

class TransitionableImage
    : public PredefinedElement<
        TransformComponent,
        SpriteComponent,
        AnimationComponent
    >
{
public:
    TransitionableImage(Element *parent, std::string name);

    void switchImage(
        double duration,
        const std::string &easing,
        std::shared_ptr<GpuImage> image);
};
}
