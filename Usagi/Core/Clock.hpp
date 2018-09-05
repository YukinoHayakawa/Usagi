#pragma once

#include <chrono>

namespace usagi
{
using TimeDuration = double;
using TimePoint = double;

class Clock
{
    using ClockT = std::chrono::high_resolution_clock;
    ClockT::time_point mStart;
    ClockT::time_point mLastTick;
    using Duration = std::chrono::duration<double>;
    Duration mSinceLastTick;
    Duration mTillLastTick;

public:
    Clock();

    void reset();

    /**
     * \brief Time since construction or reset till invocation.
     * \return
     */
    TimePoint now() const;

    /**
     * \brief Calculate elapsed time since last tick.
     * \return
     */
    TimeDuration tick();

    /**
     * \brief Time since last tick.
     * \return
     */
    TimeDuration elapsed() const;

    /**
     * \brief Time since start to last tick.
     * \return
     */
    TimeDuration totalElapsed() const;
};
}
