#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Workload/Pipeline.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/DataFormat.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/PipelineCreateInfo.hpp>

namespace yuki::extension::vulkan
{

class Pipeline : public graphics::Pipeline
{
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniqueRenderPass mRenderPass;
    vk::UniquePipeline mPipeline;

    static vk::UniqueShaderModule _createShaderModule(vk::Device device, const class graphics::SpirvShader *shader_bytecode);

public:
    static std::unique_ptr<Pipeline> create(class Device *vulkan_gd, const graphics::PipelineCreateInfo &info);

    static vk::Format translateSourceFormat(const graphics::DataFormat native_data);
    static vk::VertexInputRate translateVertexInputRate(const graphics::VertexBufferBinding::InputRate rate);
    static vk::PrimitiveTopology translateTopology(const graphics::InputAssembleState::PrimitiveTopology topology);
    static vk::CullModeFlags translateCullMode(const graphics::RasterizationState::FaceCullingMode face_culling_mode);
    static vk::FrontFace translateFrontFace(const graphics::RasterizationState::FrontFace front_face);
    static vk::PolygonMode translatePolygonMode(const graphics::RasterizationState::PolygonMode polygon_mode);
    static vk::BlendOp translateBlendOp(const graphics::ColorBlendState::BlendingOperation blending_operation);
    static vk::BlendFactor translateColorBlendFactor(const graphics::ColorBlendState::BlendingFactor blending_factor);

    vk::RenderPass _getRenderPass() const;
    vk::Pipeline _getPipeline() const;
};

}
