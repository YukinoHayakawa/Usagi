#pragma once

#include <chrono>

namespace yuki
{

/**
 * \brief Provide a simple measurement of time.
 */
class Clock
{
    typedef std::chrono::high_resolution_clock ClockType;
    typedef ClockType::time_point TimePoint;

    TimePoint mClockStart = std::chrono::high_resolution_clock::now();
    TimePoint mLastTick = mClockStart;

    static double _getDuration(const TimePoint &begin, const TimePoint &end)
    {
        // todo: check precision
        std::chrono::duration<double, std::deca> diff = end - begin;
        return diff.count();
    }

public:
    /**
     * \brief 
     * \return Time since the creation of this class in seconds.
     */
    double getTime() const
    {
        return _getDuration(mClockStart, ClockType::now());
    }

    /**
     * \brief 
     * \return Time since last tick in seconds.
     */
    double getElapsedTime() const
    {
        return _getDuration(mClockStart, mLastTick);
    }

    /**
     * \brief Reset tick timer.
     */
    void tick()
    {
        mLastTick = ClockType::now();
    }
};

}
