#pragma once

#include <cstddef>

#include "ShaderResource.hpp"

namespace usagi
{
class GpuBuffer : public ShaderResource
{
public:
    virtual ~GpuBuffer() = default;

    /**
     * \brief Release any previously allocated memory and obtain a new
     * allocation. The old allocation will be freed when all pending
     * operations involving it are finished.
     * \param size
     */
    virtual void allocate(std::size_t size) = 0;

    /**
     * \brief Indicate that the buffer is no longer used. It will live
     * till all pending operations involving it are finished.
     */
    virtual void release() = 0;

    virtual void * mappedMemory() = 0;
    virtual std::size_t size() const = 0;

    template <typename CastType>
    CastType* mappedMemory()
    {
        return reinterpret_cast<CastType *>(mappedMemory());
    }

    virtual void flush() = 0;
};
}
