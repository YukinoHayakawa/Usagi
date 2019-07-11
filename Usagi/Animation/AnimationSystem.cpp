#include "AnimationSystem.hpp"

#include <cmath>

#include <Usagi/Core/Logging.hpp>

namespace
{
using namespace usagi;

struct Sentry
{
    std::vector<Animation> &animations;
    std::vector<Animation>::iterator &i;
    bool &last_finished;

    Sentry(
        std::vector<Animation> &animations,
        std::vector<Animation>::iterator &animation,
        bool &last_finished)
        : animations(animations)
        , i(animation)
        , last_finished(last_finished)
    {
    }

    ~Sentry()
    {
        if(last_finished)
            i = animations.erase(i);
        else
            ++i;
    }
};
}

void usagi::AnimationSystem::update(const Clock &clock)
{
    mActiveCount = 0;
    for(auto &&e : mRegistry)
    {
        const auto ani = std::get<AnimationComponent*>(e.second);
        auto last_finished = true;
        for(auto i = ani->animations.begin(); i != ani->animations.end();)
        {
            Sentry sentry { ani->animations, i, last_finished };

            // check start conditions
            if(!i->started)
            {
                if(i->policy == Animation::StartPolicy::IMMEDIATELY)
                {
                    i->restart(clock.totalElapsed());
                }
                else if(i->policy == Animation::StartPolicy::SEQUENTIAL)
                {
                    if(last_finished)
                    {
                        i->restart(clock.totalElapsed());
                    }
                    else
                    {
                        last_finished = false;
                        continue;
                    }
                }
            }

            const auto rel_time = clock.totalElapsed() - i->start_time;
            if(rel_time < 0)
            {
                last_finished = false;
                continue;
            }

            mActiveCount++;

            // start TIME_POINT animation
            if(!i->started) i->start();

            assert(i->duration > 0);
            i->animation_time = rel_time / i->duration;

            const auto finished = i->animation_time > 1.0;
            // set animated object to final state and remove the animation
            if(finished && !i->loop) // finish animation
            {
                i->finish();
                mActiveCount--;

                last_finished = true;
            }
            else // run animation
            {
                double iteration;
                i->animation_time = std::modf(i->animation_time, &iteration);
                if(iteration > i->iteration) // entered next iteration
                {
                    LOG(info, "Animation iteraton #{}: {}",
                        static_cast<std::size_t>(iteration), i->name);
                    if(i->loop_callback) i->loop_callback(&*i);
                }
                i->iteration = static_cast<std::size_t>(iteration);
                i->animation_func(i->timing_func(i->animation_time));

                last_finished = false;
            }
        }
    }
}

void AnimationSystem::immediatelyFinishAll()
{
    // todo
}
