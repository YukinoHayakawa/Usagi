#include "VulkanGraphicsPipeline.hpp"

#include "VulkanRenderPass.hpp"

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
