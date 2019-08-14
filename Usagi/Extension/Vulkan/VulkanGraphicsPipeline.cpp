#include "VulkanGraphicsPipeline.hpp"

#include <Usagi/Core/Logging.hpp>

#include "VulkanRenderPass.hpp"

usagi::VulkanRenderPass * usagi::VulkanGraphicsPipeline::renderPass() const
{
    return mRenderPass.get();
}

vk::DescriptorSetLayout usagi::VulkanGraphicsPipeline::descriptorSetLayout(
    const std::uint32_t set_id) const
{
    const auto i = mLayouts.find(set_id);
    if(i == mLayouts.end())
    {
        LOG(error, "Nonexisting descriptor set id = {}", set_id);
        USAGI_THROW(std::logic_error("Referenced invalid resource."));
    }
    return mLayouts.find(set_id)->second.get();
}

vk::DescriptorType usagi::VulkanGraphicsPipeline::descriptorType(
    const std::uint32_t set_id,
    const std::uint32_t binding) const
{
    auto &bindings = mLayoutBindings.find(set_id)->second;
    // access by index if no binding is skipped
    if(binding < bindings.size())
    {
        auto &vk_binding = bindings[binding];
        if(vk_binding.binding == binding)
            return vk_binding.descriptorType;
    }
    // fallback to linear search
    const auto iter = std::find_if(bindings.begin(), bindings.end(),
        [&](auto &&b) { return b.binding == binding; });
    return iter->descriptorType;
}

usagi::VulkanPushConstantField usagi::VulkanGraphicsPipeline::queryConstantInfo(
    ShaderStage stage,
    const std::string &name) const
{
    // todo error handling
    return mConstantFieldMap.at(stage).at(name);
}
