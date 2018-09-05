#include "AnimationSubsystem.hpp"

#include <cmath>

#include <Usagi/Core/Logging.hpp>

void usagi::AnimationSubsystem::update(const Clock &clock)
{
    for(auto &&e : mRegistry)
    {
        const auto ani = std::get<AnimationComponent*>(e.second);
        for(auto i = ani->animations.begin(); i != ani->animations.end();)
        {
            const auto rel_time = clock.totalElapsed() - i->start_time;
            // animation not started
            if(rel_time < 0) continue;
            assert(i->duration > 0);
            i->animation_time = rel_time / i->duration;

            const auto finished = i->animation_time;

            // set animated object to final state and remove the animation
            if(finished && !i->loop)
            {
                i->finish();
                i = ani->animations.erase(i);
            }
            else
            {
                double iteration;
                i->animation_time = std::modf(i->animation_time, &iteration);
                if(iteration == 0) // first iteration
                {
                    LOG(debug, "Animation stated: {}", i->name);
                    if(i->begin_callback) i->begin_callback(&*i);
                }
                else if(iteration > i->iteration) // entered next iteration
                {
                    LOG(debug, "Animation looped: {}", i->name);
                    if(i->loop_callback) i->loop_callback(&*i);
                }
                i->iteration = static_cast<std::size_t>(iteration);
                i->animation_func(i->timing_func(i->animation_time));
            }
        }
    }
}
