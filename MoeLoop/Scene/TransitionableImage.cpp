#include "TransitionableImage.hpp"

#include <Usagi/Animation/AnimationComponent.hpp>
#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Core/Math.hpp>
#include <Usagi/Core/Logging.hpp>

#include <MoeLoop/Render/SpriteComponent.hpp>

namespace usagi::moeloop
{
TransitionableImage::TransitionableImage(Element *parent, std::string name)
    : PredefinedElement(parent, std::move(name))
{
}

void TransitionableImage::switchImage(
    const double duration,
    const std::string &easing,
    std::shared_ptr<GpuImage> image)
{
    Animation ani;
    ani.name = "image transition";
    ani.duration = duration;
    ani.timing_func = timing_functions::get(easing);
    ani.animation_func = [&](const double t) {
        const auto s = comp<SpriteComponent>();
        // fade in next image
        s->layers[1].factor = static_cast<float>(lerp(t, 0.0, 1.0));
        // fade out current image
        s->layers[0].factor = 1.f - s->layers[1].factor;
    };
    ani.begin_callback = [&, image](Animation *ani) {
        const auto s = comp<SpriteComponent>();
        s->show = true;
        s->layers[1].texture = std::move(image);
    };
    ani.finish_callback = [&](Animation *ani) {
        const auto s = comp<SpriteComponent>();
        // make next image current
        s->layers[0] = std::move(s->layers[1]);
    };
    comp<AnimationComponent>()->add(std::move(ani));
}
}
