#pragma once

#include <cstddef>
#include <cassert>

namespace usagi
{
template <typename T>
class ArrayView
{
    T *mArray = nullptr;
    std::size_t mSize = 0;

public:
    ArrayView(T *array, std::size_t size)
        : mArray(array)
        , mSize(size)
    {
    }

    auto operator[](const std::size_t i) const
    {
        assert(i < mSize);
        return mArray[i];
    }

    T * data() const
    {
        return mArray;
    }

    std::size_t size() const
    {
        return mSize;
    }
};
}
