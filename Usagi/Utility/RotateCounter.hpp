#pragma once

#include <cassert>

namespace usagi
{
template <typename CounterT>
class RotateCounter
{
    CounterT mWrapPoint = 0;
    CounterT mCurrent = 0;

public:
    RotateCounter() = default;

    explicit RotateCounter(CounterT wrap_point)
    {
        reset(wrap_point);
    }

    CounterT current() const
    {
        return mCurrent;
    }

    operator CounterT() const
    {
        return current();
    }

    CounterT max() const
    {
        return mWrapPoint;
    }

    CounterT & operator++()
    {
        return mCurrent = (mWrapPoint == 0 ? 0 : ++mCurrent % mWrapPoint);
    }

    CounterT operator++(int)
    {
        const auto cur = mCurrent;
        ++*this;
        return cur;
    }

    void reset(CounterT wrap_point)
    {
        assert(wrap_point >= 0);
        mWrapPoint = wrap_point;
        mCurrent = 0;
    }
};
}
