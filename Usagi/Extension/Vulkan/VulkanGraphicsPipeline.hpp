#pragma once

#include <map>

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/GraphicsPipeline.hpp>
#include <Usagi/Runtime/Graphics/Shader/ShaderStage.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanRenderPass;

struct VulkanPushConstantField
{
    std::uint32_t offset = 0, size = 0;
};

class VulkanGraphicsPipeline
    : public GraphicsPipeline
    , public VulkanBatchResource
{
public:
    using DescriptorSetLayoutBindingMap =
        std::map<std::uint32_t, std::vector<vk::DescriptorSetLayoutBinding>>;
    using DescriptorSetLayoutMap =
        std::map<std::uint32_t, vk::UniqueDescriptorSetLayout>;
    using PushConstantFieldMap =
        std::map<ShaderStage, std::map<std::string, VulkanPushConstantField>>;

private:
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;
    std::shared_ptr<VulkanRenderPass> mRenderPass;

    const DescriptorSetLayoutBindingMap mLayoutBindings;
    const DescriptorSetLayoutMap mLayouts;
    const PushConstantFieldMap mConstantFieldMap;

public:
    VulkanGraphicsPipeline(
        vk::UniquePipeline vk_pipeline,
        vk::UniquePipelineLayout vk_pipeline_layout,
        std::shared_ptr<VulkanRenderPass> vulkan_render_pass,
        DescriptorSetLayoutBindingMap layout_bindings,
        DescriptorSetLayoutMap layout,
        PushConstantFieldMap constant_field_map)
        : mPipeline { std::move(vk_pipeline) }
        , mPipelineLayout { std::move(vk_pipeline_layout) }
        , mRenderPass { std::move(vulkan_render_pass) }
        , mLayoutBindings(std::move(layout_bindings))
        , mLayouts { std::move(layout) }
        , mConstantFieldMap { std::move(constant_field_map) }
    {
    }

    vk::Pipeline pipeline() const { return mPipeline.get(); }
    vk::PipelineLayout layout() const { return mPipelineLayout.get(); }
    VulkanRenderPass * renderPass() const;

    vk::DescriptorSetLayout descriptorSetLayout(std::uint32_t set_id) const;
    vk::DescriptorType descriptorType(
        std::uint32_t set_id,
        std::uint32_t binding) const;

    VulkanPushConstantField queryConstantInfo(
        ShaderStage stage,
        const std::string &name) const;
};
}
