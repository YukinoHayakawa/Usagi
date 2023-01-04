#pragma once

namespace usagi
{
// Type tag. Used for dispatching.
template <typename T>
struct TypeTag
{
};

template <typename T>
struct BoolTag
{
    bool val;
};

template <typename T>
struct WrappedValue
{
    T val;
};
}
