#pragma once

#include <map>

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Graphics/GraphicsPipeline.hpp>
#include <Usagi/Engine/Graphics/Shader/ShaderStage.hpp>

namespace usagi
{
struct VulkanPushConstantField
{
	std::uint32_t offset, size;
};

class VulkanGraphicsPipeline : public GraphicsPipeline
{
    vk::Pipeline mPipeline;
    vk::PipelineLayout mLayout;

	std::map<ShaderStage, std::map<std::string, VulkanPushConstantField>>
		mConstantInfoMap;

public:
    vk::Pipeline pipeline() const { return mPipeline; }
    vk::PipelineLayout layout() const { return mLayout; }

    VulkanPushConstantField queryConstantInfo(
        ShaderStage stage,
        const std::string &name);
};
}
