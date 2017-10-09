#pragma once

#include <algorithm>

namespace yuki
{

template <typename T>
struct Interval
{
    T left_endpoint, right_endpoint;
};

template <typename T>
bool withinOpenInterval(const T &value, const Interval<T> &interval)
{
    return value > interval.left_endpoint && value < interval.right_endpoint;
}

template <typename T>
T clamp(const T &value, const Interval<T> &interval)
{
    return std::max(std::min(interval.right_endpoint, value), interval.left_endpoint);
}

}
