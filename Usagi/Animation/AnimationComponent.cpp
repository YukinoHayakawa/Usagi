#include "AnimationComponent.hpp"

#include <Usagi/Core/Logging.hpp>

void usagi::Animation::start()
{
    if(begin_callback) begin_callback(this);
    started = true;
    LOG(info, "Animation started: {}", name);
}

void usagi::Animation::restart(const TimePoint time)
{
    start_time = time;
    start();
}

void usagi::Animation::finish()
{
    animation_func(timing_func(1.f));
    if(finish_callback) finish_callback(this);
    LOG(info, "Animation finished: {}", name);
}

void usagi::AnimationComponent::add(Animation animation)
{
    assert(animation.duration > 0);
    LOG(debug, "Adding animation: {}", animation.name);
    animations.push_back(std::move(animation));
}

void usagi::AnimationComponent::finishAll()
{
    for(auto &&a : animations)
    {
        a.finish();
    }
    animations.clear();
}
