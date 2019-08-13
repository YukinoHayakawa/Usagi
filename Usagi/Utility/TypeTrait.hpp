#pragma once

namespace usagi
{
template <typename T>
struct DefaultConstructed
{
    constexpr static T value { };
};

template <typename T>
T DefaultConstructedValue = DefaultConstructed<T>::value;
}
