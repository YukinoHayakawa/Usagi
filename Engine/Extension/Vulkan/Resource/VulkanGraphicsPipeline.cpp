#include "VulkanGraphicsPipeline.hpp"

#include <Usagi/Engine/Extension/Vulkan/VulkanRenderPass.hpp>

vk::RenderPass usagi::VulkanGraphicsPipeline::renderPass() const
{
    return mRenderPass->renderPass();
}

usagi::VulkanPushConstantField usagi::VulkanGraphicsPipeline::queryConstantInfo(
    ShaderStage stage,
    const std::string &name) const
{
    return mConstantFieldMap.at(stage).at(name);
}
