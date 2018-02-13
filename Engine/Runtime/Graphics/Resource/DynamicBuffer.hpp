#pragma once

#include <memory>

#include "Buffer.hpp"

namespace yuki::graphics
{
class DynamicBuffer : public Buffer
{
public:
    enum class UsageFlag
    {
        // the content of the buffer changes frequently, typically pre-frame.
        // the buffer will likely be allocated in host-visible heaps to avoid
        // unnecessary copies.
        STREAM,
        // the content of the buffer can change, but only occasionally.
        // the buffer will likely be allocated in device-local heaps, the mapping
        // may be simulated in a host-side staging buffer, and unmapping may involves
        // copying from the staging buffer to the device.
        DYNAMIC,
    };

    /**
     * \brief Get an address that the host can write to. Depending on implementation,
     * the writes to the address may immediately affect the device memory or not.
     * Make sure that the buffer is not being used before writing. This is not persistent
     * mapping and the returned address should not be stored over a long period of time.
     * The address may be invalidated when the buffer is swapped.
     * \param offset 
     * \param size 
     * \return 
     */
    virtual void * map(std::size_t offset, std::size_t size) = 0;

    /**
     * \brief Flush the content of the mapped memory to the device. The resource may not
     * be immediately available to use.
     */
    virtual void unmap() = 0;

    enum class SwapFlag
    {
        // copy content of current buffer to the next buffer.
        COPY,
        // does not care about the initial content, it may be rubbish.
        UNDEFINED,
    };
};
}
