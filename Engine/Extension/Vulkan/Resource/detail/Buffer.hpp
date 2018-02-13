#pragma once

#include <utility>

#include <vulkan/vulkan.hpp>

namespace yuki::extension::vulkan
{
class Buffer
{
public:
    virtual ~Buffer() = default;

    /**
     * \brief 
     * \return <buffer handle, bind offset>
     */
    virtual std::pair<vk::Buffer, std::size_t> getBindInfo() = 0;
};
}
