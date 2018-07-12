#pragma once

#include <map>

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Graphics/GraphicsPipelineCompiler.hpp>
#include <spirv_cross.hpp>

namespace usagi
{
class VulkanGpuDevice;

class VulkanGraphicsPipelineCompiler final : public GraphicsPipelineCompiler
{
    static vk::Format translateSourceFormat(GpuDataFormat native_data);
    static vk::VertexInputRate translateVertexInputRate(
        VertexBufferBindingInfo::InputRate rate);
    static vk::PrimitiveTopology translateTopology(
        InputAssemblyState::PrimitiveTopology topology);
    static vk::CullModeFlags translateCullMode(
        RasterizationState::FaceCullingMode face_culling_mode);
    static vk::FrontFace translateFrontFace(
        RasterizationState::FrontFace front_face);
    static vk::PolygonMode translatePolygonMode(
        RasterizationState::PolygonMode polygon_mode);
    static vk::BlendOp translateBlendOp(
        ColorBlendState::BlendingOperation blending_operation);
    static vk::BlendFactor translateColorBlendFactor(
        ColorBlendState::BlendingFactor blending_factor);

    VulkanGpuDevice *mDevice = nullptr;

    SpirvBinary *mVertexShader = nullptr;
    SpirvBinary *mFragmentShader = nullptr;

    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyStateCreateInfo;
	vk::PipelineViewportStateCreateInfo mViewportStateCreateInfo;
    vk::PipelineRasterizationStateCreateInfo mRasterizationStateCreateInfo;
    vk::PipelineMultisampleStateCreateInfo mMultisampleStateCreateInfo;
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilStateCreateInfo;
    vk::PipelineColorBlendAttachmentState mColorBlendAttachmentState;
    vk::PipelineColorBlendStateCreateInfo mColorBlendStateCreateInfo;

    // obtained from shader reflection

    vk::UniquePipelineLayout mPipelineLayout;

    vk::GraphicsPipelineCreateInfo mPipelineCreateInfo;

    using VertexInputBindingArray = std::vector<
        vk::VertexInputBindingDescription>;
    VertexInputBindingArray mVertexInputBindings;

    using VertexAttributeNameMap = std::map<std::string,
        vk::VertexInputAttributeDescription>;
    VertexAttributeNameMap mVertexAttributeNameMap;

    using VertexAttributeArray = std::vector<
        vk::VertexInputAttributeDescription>;
    VertexAttributeArray mVertexAttributeIndexArray;

    /**
	 * \brief Gathers pipeline layout and vertex input information.
	 */
	void reflectShaders();

	static std::uint32_t getPushConstantSize(
		const spirv_cross::Compiler &compiler,
		spirv_cross::ShaderResources res);

public:

    VulkanGraphicsPipelineCompiler();

    void setVertexShader(SpirvBinary *shader) override;
    void setFragmentShader(SpirvBinary *shader) override;

    void setVertexBufferBinding(
        uint32_t binding_index,
        VertexBufferBindingInfo &info) override;
    void setVertexAttribute(
        std::string attr_name,
        const VertexAttribute &info) override;
    void setVertexAttribute(
        uint32_t attr_location,
        const VertexAttribute &info) override;

    void setInputAssemblyState(const InputAssemblyState &state) override;
    void setRasterizationState(const RasterizationState &state) override;
    void setDepthStencilState(const DepthStencilState &state) override;
    void setColorBlendState(const ColorBlendState &state) override;

    std::shared_ptr<GraphicsPipeline> compile() override;
};
}
