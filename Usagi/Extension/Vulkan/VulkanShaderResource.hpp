#pragma once

#include <Usagi/Utility/Noncopyable.hpp>

#include "VulkanResourceInfo.hpp"

namespace usagi
{
/**
 * \brief Interface for filling vk::WriteDescriptorSet.
 */
class VulkanShaderResource : Noncopyable
{
public:
    virtual ~VulkanShaderResource() = default;

    /**
     * \brief Set DescriptorImageInfo, DescriptorBufferInfo, or
     * BufferView depending on the resource type.
     * \param write
     * \param info
     */
    virtual void fillShaderResourceInfo(
        vk::WriteDescriptorSet &write,
        VulkanResourceInfo &info) = 0;
};
}
