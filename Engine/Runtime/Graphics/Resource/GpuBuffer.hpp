#pragma once

#include <cstddef>

#include "ShaderResource.hpp"

namespace usagi
{
class GpuBuffer : public ShaderResource
{
public:
    virtual ~GpuBuffer() = default;

    virtual void allocate(std::size_t size) = 0;
    virtual void * mappedMemory() = 0;

    template <typename CastType>
    CastType* mappedMemory()
    {
        return reinterpret_cast<CastType *>(mappedMemory());
    }

    virtual void flush() = 0;
};
}
