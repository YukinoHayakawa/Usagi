#include <Usagi/Engine/Runtime/Exception.hpp>
#include <Usagi/Engine/Runtime/Graphics/SpirvShader.hpp>

#include "Pipeline.hpp"
#include "../Device/Device.hpp"
#include "../Resource/Image.hpp"

namespace yuki::vulkan
{

vk::Format Pipeline::translateSourceFormat(const graphics::DataFormat native_data)
{
    switch(native_data)
    {
        case graphics::DataFormat::R32_SFLOAT: return vk::Format::eR32Sfloat;
        case graphics::DataFormat::R32G32_SFLOAT: return vk::Format::eR32G32Sfloat;
        case graphics::DataFormat::R32G32B32_SFLOAT: return vk::Format::eR32G32B32Sfloat;
        case graphics::DataFormat::R32G32B32A32_SFLOAT: return vk::Format::eR32G32B32A32Sfloat;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::Format");
    }
}

vk::VertexInputRate Pipeline::translateVertexInputRate(const graphics::VertexBufferBinding::InputRate rate)
{
    switch(rate)
    {
        case graphics::VertexBufferBinding::InputRate::PER_VERTEX: return vk::VertexInputRate::eVertex;
        case graphics::VertexBufferBinding::InputRate::PER_INSTANCE: return vk::VertexInputRate::eInstance;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::VertexInputRate");
    }
}

vk::PrimitiveTopology Pipeline::translateTopology(const graphics::InputAssembleState::PrimitiveTopology topology)
{
    switch(topology)
    {
        case graphics::InputAssembleState::PrimitiveTopology::POINT_LIST: return vk::PrimitiveTopology::ePointList;
        case graphics::InputAssembleState::PrimitiveTopology::LINE_LIST: return vk::PrimitiveTopology::eLineList;
        case graphics::InputAssembleState::PrimitiveTopology::LINE_STRIP: return vk::PrimitiveTopology::eLineStrip;
        case graphics::InputAssembleState::PrimitiveTopology::TRIANGLE_LIST: return vk::PrimitiveTopology::eTriangleList;
        case graphics::InputAssembleState::PrimitiveTopology::TRIANGLE_STRIP: return vk::PrimitiveTopology::eTriangleStrip;
        case graphics::InputAssembleState::PrimitiveTopology::TRIANGLE_FAN: return vk::PrimitiveTopology::eTriangleFan;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::VertexInputRate");
    }
}

vk::CullModeFlags Pipeline::translateCullMode(
    const graphics::RasterizationState::FaceCullingMode face_culling_mode)
{
    switch(face_culling_mode)
    {
        case graphics::RasterizationState::FaceCullingMode::NONE: return vk::CullModeFlagBits::eNone;
        case graphics::RasterizationState::FaceCullingMode::FRONT: return vk::CullModeFlagBits::eFront;
        case graphics::RasterizationState::FaceCullingMode::BACK: return vk::CullModeFlagBits::eBack;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::CullModeFlags");
    }
}

vk::FrontFace Pipeline::translateFrontFace(const graphics::RasterizationState::FrontFace front_face)
{
    switch(front_face)
    {
        case graphics::RasterizationState::FrontFace::CLOCKWISE: return vk::FrontFace::eClockwise;
        case graphics::RasterizationState::FrontFace::COUNTER_CLOCKWISE: return vk::FrontFace::eCounterClockwise;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::FrontFace");
    }
}

vk::PolygonMode Pipeline::translatePolygonMode(const graphics::RasterizationState::PolygonMode polygon_mode)
{
    switch(polygon_mode)
    {
        case graphics::RasterizationState::PolygonMode::FILL: return vk::PolygonMode::eFill;
        case graphics::RasterizationState::PolygonMode::LINE: return vk::PolygonMode::eLine;
        case graphics::RasterizationState::PolygonMode::POINT: return vk::PolygonMode::ePoint;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::PolygonMode");
    }
}

vk::UniqueShaderModule Pipeline::_createShaderModule(vk::Device device, const graphics::SpirvShader *shader_bytecode)
{
    vk::ShaderModuleCreateInfo shader_module_create_info;
    shader_module_create_info.setCodeSize(shader_bytecode->getByteCodeSize());
    shader_module_create_info.setPCode(shader_bytecode->getByteCode());
    auto shader_module = device.createShaderModuleUnique(shader_module_create_info);
    return shader_module;
}

vk::BlendOp Pipeline::translateBlendOp(const graphics::ColorBlendState::BlendingOperation blending_operation)
{
    switch(blending_operation)
    {
        case graphics::ColorBlendState::BlendingOperation::ADD: return vk::BlendOp::eAdd;
        case graphics::ColorBlendState::BlendingOperation::SUBTRACT: return vk::BlendOp::eSubtract;
        case graphics::ColorBlendState::BlendingOperation::REVERSE_SUBTRACT: return vk::BlendOp::eReverseSubtract;
        case graphics::ColorBlendState::BlendingOperation::MIN: return vk::BlendOp::eMin;
        case graphics::ColorBlendState::BlendingOperation::MAX: return vk::BlendOp::eMax;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::BlendOp");
    }
}

vk::BlendFactor Pipeline::translateColorBlendFactor(
    const graphics::ColorBlendState::BlendingFactor blending_factor)
{
    switch(blending_factor)
    {
        case graphics::ColorBlendState::BlendingFactor::ZERO: return vk::BlendFactor::eZero;
        case graphics::ColorBlendState::BlendingFactor::ONE: return vk::BlendFactor::eOne;
        case graphics::ColorBlendState::BlendingFactor::SOURCE_ALPHA: return vk::BlendFactor::eSrcAlpha;
        case graphics::ColorBlendState::BlendingFactor::ONE_MINUS_SOURCE_ALPHA: return vk::BlendFactor::eOneMinusSrcAlpha;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::BlendFactor");
    }
}

vk::RenderPass Pipeline::_getRenderPass() const
{
    return mRenderPass.get();
}

vk::Pipeline Pipeline::_getPipeline() const
{
    return mPipeline.get();
}

std::unique_ptr<Pipeline> Pipeline::create(class Device *vulkan_gd, const graphics::PipelineCreateInfo &info)
{
    auto pipeline = std::make_unique<Pipeline>();

    vk::GraphicsPipelineCreateInfo pipeline_create_info;

    // shader stages
    enum ShaderStage
    {
        VERTEX, FRAGMENT, SHADER_STAGE_COUNT
    };
    vk::PipelineShaderStageCreateInfo shader_stage_create_info[SHADER_STAGE_COUNT];
    auto vertex_shader = _createShaderModule(vulkan_gd->_getDevice(), info.vertex_shader.get());
    shader_stage_create_info[VERTEX].setModule(vertex_shader.get());
    shader_stage_create_info[VERTEX].setPName("main"); // entry point name
    shader_stage_create_info[VERTEX].setStage(vk::ShaderStageFlagBits::eVertex);
    auto fragment_shader = _createShaderModule(vulkan_gd->_getDevice(), info.fragment_shader.get());
    shader_stage_create_info[FRAGMENT].setModule(fragment_shader.get());
    shader_stage_create_info[FRAGMENT].setPName("main"); // entry point name
    shader_stage_create_info[FRAGMENT].setStage(vk::ShaderStageFlagBits::eFragment);
    pipeline_create_info.setStageCount(SHADER_STAGE_COUNT);
    pipeline_create_info.setPStages(shader_stage_create_info);

    // vertex input
    vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info;
    std::vector<vk::VertexInputBindingDescription> vertex_input_binding;
    for(auto &&binding : info.vertex_input.bindings)
    {
        vk::VertexInputBindingDescription vulkan_binding;
        vulkan_binding.setBinding(binding.binding_slot);
        vulkan_binding.setStride(binding.stride);
        vulkan_binding.setInputRate(translateVertexInputRate(binding.input_rate));
        vertex_input_binding.push_back(std::move(vulkan_binding));
    }
    std::vector<vk::VertexInputAttributeDescription> vertex_input_attribute;
    for(auto &&attr : info.vertex_input.attributes)
    {
        vk::VertexInputAttributeDescription vulkan_attr;
        vulkan_attr.setLocation(attr.location);
        vulkan_attr.setBinding(attr.binding_slot);
        vulkan_attr.setOffset(attr.offset);
        vulkan_attr.setFormat(translateSourceFormat(attr.source_format));
        vertex_input_attribute.push_back(std::move(vulkan_attr));
    }
    vertex_input_state_create_info.setVertexBindingDescriptionCount(vertex_input_binding.size());
    vertex_input_state_create_info.setPVertexBindingDescriptions(vertex_input_binding.data());
    vertex_input_state_create_info.setVertexAttributeDescriptionCount(vertex_input_attribute.size());
    vertex_input_state_create_info.setPVertexAttributeDescriptions(vertex_input_attribute.data());
    pipeline_create_info.setPVertexInputState(&vertex_input_state_create_info);

    // input assembly
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;
    input_assembly_state_create_info.setTopology(translateTopology(info.input_assembly.topology));
    pipeline_create_info.setPInputAssemblyState(&input_assembly_state_create_info);

    // tessellation

    // viewport
    vk::PipelineViewportStateCreateInfo viewport_state_create_info;
    // these two are set as dynamic states in command lists
    viewport_state_create_info.setViewportCount(1);
    viewport_state_create_info.setScissorCount(1);
    pipeline_create_info.setPViewportState(&viewport_state_create_info);

    // rasterization
    vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info;
    rasterization_state_create_info.setDepthBiasEnable(false);
    rasterization_state_create_info.setCullMode(translateCullMode(info.rasterization.face_culling_mode));
    rasterization_state_create_info.setFrontFace(translateFrontFace(info.rasterization.front_face));
    rasterization_state_create_info.setPolygonMode(translatePolygonMode(info.rasterization.polygon_mode));
    rasterization_state_create_info.setLineWidth(1.f);
    pipeline_create_info.setPRasterizationState(&rasterization_state_create_info);

    // multi-sampling
    // todo: anti-aliasing
    vk::PipelineMultisampleStateCreateInfo multisample_state_create_info;
    multisample_state_create_info.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisample_state_create_info.setMinSampleShading(1.f);
    pipeline_create_info.setPMultisampleState(&multisample_state_create_info);

    // depth stencil

    // color blend
    vk::PipelineColorBlendAttachmentState color_blend_attachment_state;
    color_blend_attachment_state.setColorBlendOp(translateBlendOp(info.color_blend.color_blend_op));
    color_blend_attachment_state.setAlphaBlendOp(translateBlendOp(info.color_blend.alpha_blend_op));
    color_blend_attachment_state.setSrcColorBlendFactor(translateColorBlendFactor(info.color_blend.src_color_factor));
    color_blend_attachment_state.setDstColorBlendFactor(translateColorBlendFactor(info.color_blend.dst_color_factor));
    color_blend_attachment_state.setSrcAlphaBlendFactor(translateColorBlendFactor(info.color_blend.src_alpha_factor));
    color_blend_attachment_state.setDstAlphaBlendFactor(translateColorBlendFactor(info.color_blend.dst_alpha_factor));
    color_blend_attachment_state.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info;
    color_blend_state_create_info.setAttachmentCount(1);
    color_blend_state_create_info.setPAttachments(&color_blend_attachment_state);
    pipeline_create_info.setPColorBlendState(&color_blend_state_create_info);

    // dynamic states
    // viewport and scissor should be set after binding the pipeline
    std::array<vk::DynamicState, 2> dynamic_state{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamic_state_create_info;
    dynamic_state_create_info.dynamicStateCount = dynamic_state.size();
    dynamic_state_create_info.pDynamicStates = dynamic_state.data();
    pipeline_create_info.pDynamicState = &dynamic_state_create_info;

    // pipeline layout
    vk::PipelineLayoutCreateInfo pipeline_layout_create_info;
    pipeline->mPipelineLayout = vulkan_gd->_getDevice().createPipelineLayoutUnique(pipeline_layout_create_info);
    pipeline_create_info.setLayout(pipeline->mPipelineLayout.get());

    // render pass
    std::vector<vk::AttachmentDescription> attachment_descriptions;
    std::vector<vk::AttachmentReference> attachment_references;
    for(auto &&attachment : info.attachment_usages)
    {
        vk::AttachmentDescription attachment_description; // all framebuffers available to the whole pass
        attachment_description.setFormat(static_cast<vk::Format>(attachment.native_format));
        attachment_description.setSamples(vk::SampleCountFlagBits::e1);
        attachment_description.setLoadOp(vk::AttachmentLoadOp::eClear);
        attachment_description.setStoreOp(vk::AttachmentStoreOp::eStore);
        attachment_description.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachment_description.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachment_description.setInitialLayout(Image::translateImageLayout(attachment.initial_layout));
        attachment_description.setFinalLayout(Image::translateImageLayout(attachment.final_layout));
        vk::AttachmentReference attachment_reference{ static_cast<uint32_t>(attachment_descriptions.size()), vk::ImageLayout::eColorAttachmentOptimal };
        attachment_descriptions.push_back(std::move(attachment_description));
        attachment_references.push_back(std::move(attachment_reference));
    }

    vk::SubpassDescription subpass_description;
    subpass_description.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass_description.setColorAttachmentCount(attachment_references.size());
    subpass_description.setPColorAttachments(attachment_references.data());

    std::vector<vk::SubpassDependency> subpass_dependencies{ 2, { } };
    subpass_dependencies[0].setSrcSubpass(VK_SUBPASS_EXTERNAL);
    subpass_dependencies[0].setDstSubpass(0);
    subpass_dependencies[0].setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
    subpass_dependencies[0].setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    subpass_dependencies[0].setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);
    subpass_dependencies[0].setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
    subpass_dependencies[0].setDependencyFlags(vk::DependencyFlagBits::eByRegion);

    subpass_dependencies[1].setSrcSubpass(0);
    subpass_dependencies[1].setDstSubpass(VK_SUBPASS_EXTERNAL);
    subpass_dependencies[1].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    subpass_dependencies[1].setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
    subpass_dependencies[1].setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
    subpass_dependencies[1].setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
    subpass_dependencies[1].setDependencyFlags(vk::DependencyFlagBits::eByRegion);

    vk::RenderPassCreateInfo render_pass_create_info;
    render_pass_create_info.setAttachmentCount(attachment_descriptions.size());
    render_pass_create_info.setPAttachments(attachment_descriptions.data());
    render_pass_create_info.setSubpassCount(1);
    render_pass_create_info.setPSubpasses(&subpass_description);
    render_pass_create_info.setDependencyCount(subpass_dependencies.size());
    render_pass_create_info.setPDependencies(subpass_dependencies.data());
    pipeline->mRenderPass = vulkan_gd->_getDevice().createRenderPassUnique(render_pass_create_info);

    pipeline_create_info.setRenderPass(pipeline->mRenderPass.get());
    pipeline_create_info.setSubpass(0);

    pipeline->mPipeline = vulkan_gd->_getDevice().createGraphicsPipelineUnique({ }, pipeline_create_info);

    return std::move(pipeline);
}

}
