#include "Clock.hpp"

#include "Logging.hpp"

usagi::Clock::Clock()
{
    if constexpr(!ClockT::is_steady)
        LOG(warn, "std::chrono::high_resolution_clock is not steady!");
    reset();
}

void usagi::Clock::reset()
{
    mStart = mLastTick = ClockT::now();
    mTillLastTick = mSinceLastTick = mLastTick - mStart;
}

usagi::TimePoint usagi::Clock::now() const
{
    Duration d = ClockT::now() - mStart;
    return d.count();
}

usagi::TimeDuration usagi::Clock::tick()
{
    const auto this_tick = ClockT::now();
    mSinceLastTick = this_tick - mLastTick;
    mLastTick = this_tick;
    mTillLastTick = mLastTick - mStart;
    return elapsed();
}

usagi::TimeDuration usagi::Clock::elapsed() const
{
    return mSinceLastTick.count();
}

usagi::TimeDuration usagi::Clock::totalElapsed() const
{
    return mTillLastTick.count();
}
