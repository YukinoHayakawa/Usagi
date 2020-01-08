#pragma once

#include <Usagi/Core/Clock.hpp>
#include <Usagi/Core/Logging.hpp>

namespace usagi
{
class PrintTimer
{
    Clock mClock;

public:
    ~PrintTimer()
    {
        LOG(info, "Total elapsed time: {}", mClock.tick());
    }
};
}
