#pragma once

#include <map>

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/GraphicsPipelineCompiler.hpp>

namespace usagi
{
class VulkanGraphicsPipeline;
class VulkanRenderPass;
class VulkanGpuDevice;

class VulkanGraphicsPipelineCompiler final : public GraphicsPipelineCompiler
{
    VulkanGpuDevice *mDevice = nullptr;

    struct ShaderInfo
    {
        std::string entry_point = "main";
        std::shared_ptr<SpirvBinary> binary = nullptr;
        vk::UniqueShaderModule module;
    };
    using ShaderMap = std::map<ShaderStage, ShaderInfo>;
    ShaderMap mShaders;

	vk::UniqueShaderModule createShaderModule(const SpirvBinary *binary) const;

    std::vector<vk::PipelineShaderStageCreateInfo> mShaderStageCreateInfos;
    vk::PipelineVertexInputStateCreateInfo mVertexInputStateCreateInfo;
    std::vector<vk::DynamicState> mDynamicStates;
    vk::PipelineDynamicStateCreateInfo mDynamicStateCreateInfo;
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyStateCreateInfo;
	vk::PipelineViewportStateCreateInfo mViewportStateCreateInfo;
    vk::PipelineRasterizationStateCreateInfo mRasterizationStateCreateInfo;
    vk::PipelineMultisampleStateCreateInfo mMultisampleStateCreateInfo;
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilStateCreateInfo;
    vk::PipelineColorBlendAttachmentState mColorBlendAttachmentState;
    vk::PipelineColorBlendStateCreateInfo mColorBlendStateCreateInfo;
    std::shared_ptr<VulkanRenderPass> mRenderPass;

    vk::GraphicsPipelineCreateInfo mPipelineCreateInfo;

    struct Context;
    struct ReflectionHelper;

    using VertexInputBindingArray = std::vector<
        vk::VertexInputBindingDescription>;
    VertexInputBindingArray mVertexInputBindings;

    using VertexAttributeNameMap = std::map<std::string,
        vk::VertexInputAttributeDescription>;
    // merge into the index array below before compilation
    VertexAttributeNameMap mVertexAttributeNameMap;

    using VertexAttributeLocationArray = std::vector<
        vk::VertexInputAttributeDescription>;
    VertexAttributeLocationArray mVertexAttributeLocationArray;

	void setupShaderStages();
    void setupVertexInput();
    void setupDynamicStates();

    // Pipeline derivatives support
    std::shared_ptr<VulkanGraphicsPipeline> mParentPipeline;

public:
    explicit VulkanGraphicsPipelineCompiler(VulkanGpuDevice *device);

    void setRenderPass(std::shared_ptr<RenderPass> render_pass) override;

    void setShader(
        ShaderStage stage,
        std::shared_ptr<SpirvBinary> shader) override;

    void setVertexBufferBinding(
        std::uint32_t binding_index,
        std::uint32_t stride,
        VertexInputRate input_rate) override;
    void setVertexAttribute(
        std::string attr_name,
        std::uint32_t binding_index,
        std::uint32_t offset,
        GpuBufferFormat source_format) override;
    void setVertexAttribute(
        std::uint32_t attr_location,
        std::uint32_t binding_index,
        std::uint32_t offset,
        GpuBufferFormat source_format) override;

    void iaSetPrimitiveTopology(PrimitiveTopology topology) override;
    void setInputAssemblyState(const InputAssemblyState &state) override;

    void rsSetPolygonmode(PolygonMode mode) override;
    void rsSetFaceCullingMode(FaceCullingMode mode) override;
    void rsSetFrontFace(FrontFace face) override;
    void setRasterizationState(const RasterizationState &state) override;

    void omSetDepthEnabled(bool enabled) override;
    void setDepthStencilState(const DepthStencilState &state) override;

    void omSetColorBlendEnabled(bool enabled) override;
    void setColorBlendState(const ColorBlendState &state) override;

    std::shared_ptr<GraphicsPipeline> compile() override;
};
}
