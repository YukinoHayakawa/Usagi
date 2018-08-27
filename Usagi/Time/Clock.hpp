#pragma once

#include <chrono>

namespace yuki
{

/**
 * \brief Provide a simple measurement of time.
 * 
 * todo: the precision of double changes over time, use a data type with invariant precision instead.
 */
class Clock
{
    typedef std::chrono::high_resolution_clock Time;
    typedef Time::time_point TimePoint;

    TimePoint mClockStart = Time::now();
    TimePoint mLastTick = mClockStart;

    static double _getDuration(const TimePoint &begin, const TimePoint &end)
    {
        // todo: check precision
        std::chrono::duration<double> diff = end - begin;
        return diff.count();
    }

public:
    /**
     * \brief 
     * \return Time since the creation of this class in seconds.
     */
    double getTotalElapsedTime() const
    {
        return _getDuration(mClockStart, Time::now());
    }

    /**
     * \brief Reset tick timer and return elapsed time.
     */
    double tick()
    {
        const auto now = Time::now();
        const double duration = _getDuration(mLastTick, now);
        mLastTick = now;
        return duration;
    }

    // todo remove
    double getTimeSinceLastTick() const { return 0; }
};

}
