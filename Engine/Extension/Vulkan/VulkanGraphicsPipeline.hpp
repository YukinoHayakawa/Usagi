#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsPipeline.hpp>

namespace yuki
{

class VulkanGraphicsPipeline : public GraphicsPipeline
{
    std::shared_ptr<class VulkanGraphicsDevice> mVulkanGD;

    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniqueRenderPass mRenderPass;
    vk::UniquePipeline mPipeline;

    static vk::VertexInputRate _translateVertexInputRate(VertexBufferBinding::InputRate rate);
    static vk::PrimitiveTopology _translateTopology(const InputAssembleState::PrimitiveTopology topology);
    static vk::CullModeFlags _translateCullMode(const RasterizationState::FaceCullingMode face_culling_mode);
    static vk::FrontFace _translateFrontFace(const RasterizationState::FrontFace front_face);
    static vk::PolygonMode _translatePolygonMode(const RasterizationState::PolygonMode polygon_mode);
    static vk::BlendOp _translateBlendOp(const ColorBlendState::BlendingOperation blending_operation);
    static vk::BlendFactor _translateColorBlendFactor(const ColorBlendState::BlendingFactor blending_factor);

    vk::UniqueShaderModule _createShaderModule(const SPIRVShader *shader_bytecode) const;

public:
    // todo remove format param
    virtual void init(const GraphicsPipelineCreateInfo &info, uint64_t render_target_format);
};

}
