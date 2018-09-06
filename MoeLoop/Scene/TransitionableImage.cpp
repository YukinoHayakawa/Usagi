#include "TransitionableImage.hpp"

#include <Usagi/Animation/AnimationComponent.hpp>
#include <Usagi/Core/Math.hpp>
#include <Usagi/Core/Logging.hpp>

#include <MoeLoop/Render/SpriteComponent.hpp>

#include "ImageFrame.hpp"

namespace usagi::moeloop
{
TransitionableImage::TransitionableImage(Element *parent, std::string name)
    : PredefinedElement(parent, std::move(name))
{
    mCurrentImage = addChild<ImageFrame>("current");
    mTargetImage = addChild<ImageFrame>("next");
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
        const auto current = mCurrentImage->comp<SpriteComponent>();
        const auto next = mTargetImage->comp<SpriteComponent>();
        next->fade = static_cast<float>(lerp(t, 0.0, 1.0));
        current->fade = 1.f - next->fade;
        LOG(info, "c: {} {}, t: {} {}",
            current->show, current->fade, next->show, next->fade);
    };
    ani.begin_callback = [&, image](Animation *ani) {
        const auto next_sprite = mTargetImage->comp<SpriteComponent>();
        next_sprite->show = mCurrentImage->comp<SpriteComponent>()->show = true;
        next_sprite->texture = std::move(image);
    };
    ani.finish_callback = [&](Animation *ani) {
        std::swap(mCurrentImage, mTargetImage);
        const auto next_sprite = mTargetImage->comp<SpriteComponent>();
        next_sprite->texture.reset();
        next_sprite->show = false;
    };
    comp<AnimationComponent>()->add(std::move(ani));
}
}
