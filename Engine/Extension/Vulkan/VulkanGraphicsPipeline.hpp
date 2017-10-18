#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsPipeline.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsBufferElementType.hpp>

namespace yuki
{

class VulkanGraphicsPipeline : public GraphicsPipeline
{
    class VulkanGraphicsDevice *mVulkanGD;

    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniqueRenderPass mRenderPass;
    vk::UniquePipeline mPipeline;

    vk::UniqueShaderModule _createShaderModule(const SPIRVShader *shader_bytecode) const;

public:
    VulkanGraphicsPipeline(VulkanGraphicsDevice *vulkan_graphics_device);

    void create(const GraphicsPipelineCreateInfo &info) override;

    static vk::Format translateSourceFormat(const GraphicsBufferElementType native_data);
    static vk::VertexInputRate translateVertexInputRate(const VertexBufferBinding::InputRate rate);
    static vk::PrimitiveTopology translateTopology(const InputAssembleState::PrimitiveTopology topology);
    static vk::CullModeFlags translateCullMode(const RasterizationState::FaceCullingMode face_culling_mode);
    static vk::FrontFace translateFrontFace(const RasterizationState::FrontFace front_face);
    static vk::PolygonMode translatePolygonMode(const RasterizationState::PolygonMode polygon_mode);
    static vk::BlendOp translateBlendOp(const ColorBlendState::BlendingOperation blending_operation);
    static vk::BlendFactor translateColorBlendFactor(const ColorBlendState::BlendingFactor blending_factor);

    vk::RenderPass _getRenderPass() const;
    vk::Pipeline _getPipeline() const;
};

}
