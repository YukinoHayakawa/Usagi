#pragma once

#include <cstddef>

namespace usagi
{
class GpuBuffer
{
public:
    virtual ~GpuBuffer() = default;

    GpuBuffer() = default;
	GpuBuffer(const GpuBuffer &other) = default;
	GpuBuffer(GpuBuffer &&other) = default;
	GpuBuffer & operator=(const GpuBuffer &other) = default;
	GpuBuffer & operator=(GpuBuffer &&other) = default;

    virtual void reallocate(std::size_t size) = 0;
    virtual void * getMappedMemory() = 0;

    template <typename CastType>
    CastType* getMappedMemory()
    {
        return reinterpret_cast<CastType *>(getMappedMemory());
    }

    virtual void flush() = 0;
};
}
