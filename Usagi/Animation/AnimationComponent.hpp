#pragma once

#include <cassert>
#include <string>

#include <Usagi/Core/Component.hpp>
#include <Usagi/Core/Clock.hpp>

#include "TimingFunctions.hpp"

namespace usagi
{
struct Animation
{
    std::string name;

    enum class StartPolicy
    {
        // as soon as the animation system processes this animation
        IMMEDIATELY,
        // immediately after the time point specified by start_time pasts
        TIME_POINT,
        // immediately after last animation finishes in the insertion order
        SEQUENTIAL,
    } policy = StartPolicy::IMMEDIATELY;
    TimePoint start_time = 0;
    TimeDuration duration = 0;

    // used by AnimationSubsystem

    bool started = false;
    // normalized 0~1
    TimePoint animation_time = 0;
    // loop counter
    std::size_t iteration = 0;

    /**
    * \brief If is false, this animation will be removed once finished.
    * Otherwise the animation will loop when finished.
    */
    bool loop = false;

    /**
     * \brief Input is the linearly interpolated value from 0 to 1, where
     * 0 represent the animation start and 1 the end. Output is a value to
     * be used to call animation_func, generally should be in 0 to 1, but can
     * slightly exceed the range for achieving special effects.
     */
    TimingFunction timing_func = timing_functions::LINEAR;

    using AnimationFunction = std::function<void(double t)>;

    /**
    * \brief At each frame this function is called with a value
    * generated using the timing function. It may exceed the 0-1 range
    * depending on the timing function used.
    */
    AnimationFunction animation_func;

    using Callback = std::function<void(Animation *)>;
    Callback begin_callback;
    Callback loop_callback;
    Callback finish_callback;

    void start();
    void restart(TimePoint time);
    void finish();
};

struct AnimationComponent : Component
{
    std::vector<Animation> animations;

    void add(Animation animation)
    {
        assert(animation.duration > 0);
        animations.push_back(std::move(animation));
    }

    /**
     * \brief Immediately call all animation_func with value 1 to set them
     * in finished states, then remove all animations.
     */
    void finishAll();

    const std::type_info & baseType() override
    {
        return typeid(AnimationComponent);
    }
};
}
