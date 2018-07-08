#pragma once

namespace usagi
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
}
