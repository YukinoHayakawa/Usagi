#pragma once

#include <utility>

#include <vulkan/vulkan.hpp>

namespace yuki::extension::vulkan
{
class Buffer
{
public:
    virtual ~Buffer() = default;

    struct BindInfo
    {
        vk::Buffer buffer;
        std::size_t bind_offset = 0;
        vk::Semaphore available_semaphore;
    };

    /**
     * \brief Obtain the buffer with the latest data along with other information
     * required to use it.
     * \return 
     */
    virtual BindInfo getLatestBindInfo() = 0;
};
}
