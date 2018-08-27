#pragma once

#include <vector>

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
/**
 * \brief Base class for tracking device resource usage using reference-
 * counting.
 */
class VulkanBatchResource : Noncopyable
{
public:
    virtual ~VulkanBatchResource() = default;

    virtual void appendAdditionalResources(
        std::vector<std::shared_ptr<VulkanBatchResource>> &resources) { }
};
}
