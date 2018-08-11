#pragma once

#include <cstddef>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuBuffer : Noncopyable
{
public:
    virtual ~GpuBuffer() = default;

    virtual void allocate(std::size_t size) = 0;
    virtual void * getMappedMemory() = 0;

    template <typename CastType>
    CastType* getMappedMemory()
    {
        return reinterpret_cast<CastType *>(getMappedMemory());
    }

    virtual void flush() = 0;
};
}
