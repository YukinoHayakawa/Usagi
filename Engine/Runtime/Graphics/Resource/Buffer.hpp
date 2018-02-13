#pragma once

#include <cstddef>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{
/**
 * \brief 
 * Represent a GPU memory object. Support streaming content to the device
 * without stalling the rendering pipeline.
 */
class Buffer : Noncopyable
{
public:
    virtual ~Buffer() = default;

    virtual std::size_t size() const = 0;
};
}
