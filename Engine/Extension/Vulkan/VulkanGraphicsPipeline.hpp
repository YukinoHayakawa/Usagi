#pragma once

#include <map>

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsPipeline.hpp>
#include <Usagi/Engine/Runtime/Graphics/Shader/ShaderStage.hpp>

namespace usagi
{
class VulkanRenderPass;

struct VulkanPushConstantField
{
	std::uint32_t offset = 0, size = 0;
};

class VulkanGraphicsPipeline : public GraphicsPipeline
{
public:
	using DescriptorSetLayoutMap =
		std::map<std::uint32_t, vk::UniqueDescriptorSetLayout>;
	using PushConstantFieldMap =
		std::map<ShaderStage, std::map<std::string, VulkanPushConstantField>>;

private:
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;
    std::shared_ptr<VulkanRenderPass> mRenderPass;

    const DescriptorSetLayoutMap mLayout;
	const PushConstantFieldMap mConstantFieldMap;

public:
    VulkanGraphicsPipeline(
        vk::UniquePipeline vk_pipeline,
        vk::UniquePipelineLayout vk_pipeline_layout,
        std::shared_ptr<VulkanRenderPass> vulkan_render_pass,
        DescriptorSetLayoutMap layout,
        PushConstantFieldMap constant_field_map)
        : mPipeline { std::move(vk_pipeline) }
        , mPipelineLayout { std::move(vk_pipeline_layout) }
        , mRenderPass { std::move(vulkan_render_pass) }
        , mLayout { std::move(layout) }
        , mConstantFieldMap { std::move(constant_field_map) }
    {
    }

    vk::Pipeline pipeline() const { return mPipeline.get(); }
    vk::PipelineLayout layout() const { return mPipelineLayout.get(); }
    vk::RenderPass renderPass() const;

    VulkanPushConstantField queryConstantInfo(
        ShaderStage stage,
        const std::string &name) const;
};
}
