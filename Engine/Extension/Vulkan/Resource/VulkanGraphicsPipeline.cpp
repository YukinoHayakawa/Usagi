#include "VulkanGraphicsPipeline.hpp"

#include <Usagi/Engine/Extension/Vulkan/VulkanRenderPass.hpp>

usagi::VulkanRenderPass * usagi::VulkanGraphicsPipeline::renderPass() const
{
    return mRenderPass.get();
}

usagi::VulkanPushConstantField usagi::VulkanGraphicsPipeline::queryConstantInfo(
    ShaderStage stage,
    const std::string &name) const
{
    return mConstantFieldMap.at(stage).at(name);
}
