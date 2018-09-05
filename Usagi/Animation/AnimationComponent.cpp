#include "AnimationComponent.hpp"

#include <Usagi/Core/Logging.hpp>

void usagi::Animation::finish()
{
    animation_func(timing_func(1.f));
    if(finish_callback) finish_callback(this);
    LOG(info, "Animation finished: {}", name);
}

void usagi::AnimationComponent::finishAll()
{
    for(auto &&a : animations)
    {
        a.finish();
    }
    animations.clear();
}
