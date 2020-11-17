#pragma once

namespace usagi
{
// Type tag. Used for dispatching.
template <typename T>
struct Tag
{
};

template <typename T>
struct BoolTag
{
    bool val;
};
}
