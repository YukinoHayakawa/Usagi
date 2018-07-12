#include "VulkanGraphicsPipelineCompiler.hpp"

#include <loguru.hpp>

#include <Usagi/Engine/Graphics/Shader/SpirvBinary.hpp>

#include "VulkanGpuDevice.hpp"
#include "VulkanGraphicsPipeline.hpp"

using namespace spirv_cross;

std::uint32_t usagi::VulkanGraphicsPipelineCompiler::getPushConstantSize(
    const Compiler &compiler,
    ShaderResources res)
{
    LOG_SCOPE_F(INFO, "Push constant ranges:");
    std::size_t size = 0;
    for(const auto &resource : res.push_constant_buffers)
    {
        const auto &type = compiler.get_type(resource.base_type_id);
        size             = compiler.get_declared_struct_size(type);

        LOG_SCOPE_F(INFO, "Members:");
        const auto member_count = type.member_types.size();
        for(unsigned i          = 0; i < member_count; i++)
        {
            const auto &name =
                compiler.get_member_name(type.self, i);
            const auto offset =
                compiler.type_struct_member_offset(type, i);
            const auto member_size =
                compiler.get_declared_struct_member_size(type, i);
            LOG_F(INFO, "%s: offset=%zu, size=%zu",
                name.c_str(), offset, member_size);
        }
        break; // only process first push constant block
    }
    return size;
}

void usagi::VulkanGraphicsPipelineCompiler::reflectShaders()
{
    LOG_F(INFO, "Generating pipeline layout from shaders...");

    const auto &compiler = mVertexShader->getReflectionCompiler();
    const auto res       = compiler.get_shader_resources();

    std::vector<vk::PushConstantRange> push_constants;
    std::size_t offset = 0;

    {
        getPushConstantSize(compiler, res);
    }
    vk::PipelineLayoutCreateInfo pipeline_layout_create_info;

    // Generate PipelineLayout
    mPipelineLayout = mDevice->device().createPipelineLayoutUnique(
        pipeline_layout_create_info);
    pipeline_layout_create_info.setLayout(compatible_pipeline_layout.get());
}

usagi::VulkanGraphicsPipelineCompiler::VulkanGraphicsPipelineCompiler()
{
    // IA
    mPipelineCreateInfo.setPInputAssemblyState(&mInputAssemblyStateCreateInfo);
    setInputAssemblyState({ });

    // Viewport (set as dynamic state in command lists)
    // todo: multiple viewports
    mViewportStateCreateInfo.setViewportCount(1);
    mViewportStateCreateInfo.setScissorCount(1);
    mPipelineCreateInfo.setPViewportState(&mViewportStateCreateInfo);

    // Rasterization
    mPipelineCreateInfo.setPRasterizationState(&mRasterizationStateCreateInfo);
    setRasterizationState({ });

    // Multi-sampling
    // todo: anti-aliasing
    mMultisampleStateCreateInfo.setRasterizationSamples(
        vk::SampleCountFlagBits::e1);
    mMultisampleStateCreateInfo.setMinSampleShading(1.f);
    mPipelineCreateInfo.setPMultisampleState(&mMultisampleStateCreateInfo);

    // Depth/stencil
    // todo

    // Blending
    // todo: multiple attachments
    mColorBlendStateCreateInfo.setAttachmentCount(1);
    mColorBlendStateCreateInfo.setPAttachments(&mColorBlendAttachmentState);
    mPipelineCreateInfo.setPColorBlendState(&mColorBlendStateCreateInfo);
    setColorBlendState({ });
}

void usagi::VulkanGraphicsPipelineCompiler::setVertexShader(
    SpirvBinary *shader) { mVertexShader = shader; }

void usagi::VulkanGraphicsPipelineCompiler::setFragmentShader(
    SpirvBinary *shader) { mFragmentShader = shader; }

void usagi::VulkanGraphicsPipelineCompiler::setVertexBufferBinding(
    uint32_t binding_index,
    VertexBufferBindingInfo &info)
{
    vk::VertexInputBindingDescription vulkan_binding;
    vulkan_binding.setBinding(binding_index);
    vulkan_binding.setStride(info.stride);
    vulkan_binding.setInputRate(translateVertexInputRate(info.input_rate));

    const auto iter = std::find_if(
        mVertexInputBindings.begin(), mVertexInputBindings.end(),
        [=](auto &&item) { return item.binding == binding_index; }
    );
    if(iter != mVertexInputBindings.end())
        *iter = vulkan_binding; // update old info
    else
        mVertexInputBindings.push_back(vulkan_binding);
}

void usagi::VulkanGraphicsPipelineCompiler::setVertexAttribute(
    std::string attr_name,
    const VertexAttribute &info)
{
    vk::VertexInputAttributeDescription vulkan_attr;
    // Deferred to compilation when shader reflection is available.
    // vulkan_attr.setLocation();
    vulkan_attr.setBinding(info.binding_index);
    vulkan_attr.setOffset(info.offset);
    vulkan_attr.setFormat(translateSourceFormat(info.source_format));
    mVertexAttributeNameMap.insert(std::make_pair(attr_name, vulkan_attr));
}

void usagi::VulkanGraphicsPipelineCompiler::setVertexAttribute(
    uint32_t attr_location,
    const VertexAttribute &info)
{
    vk::VertexInputAttributeDescription vulkan_attr;
    vulkan_attr.setLocation(attr_location);
    vulkan_attr.setBinding(info.binding_index);
    vulkan_attr.setOffset(info.offset);
    vulkan_attr.setFormat(translateSourceFormat(info.source_format));
    mVertexAttributeIndexArray.push_back(vulkan_attr);
}

void usagi::VulkanGraphicsPipelineCompiler::setInputAssemblyState(
    const InputAssemblyState &state)
{
    mInputAssemblyStateCreateInfo.setTopology(
        translateTopology(state.topology));
}

void usagi::VulkanGraphicsPipelineCompiler::setRasterizationState(
    const RasterizationState &state)
{
    mRasterizationStateCreateInfo.setDepthBiasEnable(false);
    mRasterizationStateCreateInfo.setCullMode(
        translateCullMode(state.face_culling_mode));
    mRasterizationStateCreateInfo.setFrontFace(
        translateFrontFace(state.front_face));
    mRasterizationStateCreateInfo.setPolygonMode(
        translatePolygonMode(state.polygon_mode));
    mRasterizationStateCreateInfo.setLineWidth(1.f);
}

void usagi::VulkanGraphicsPipelineCompiler::setDepthStencilState(
    const DepthStencilState &state) {}

void usagi::VulkanGraphicsPipelineCompiler::setColorBlendState(
    const ColorBlendState &state)
{
    mColorBlendAttachmentState.setColorBlendOp(
        translateBlendOp(state.color_blend_op));
    mColorBlendAttachmentState.setAlphaBlendOp(
        translateBlendOp(state.alpha_blend_op));
    mColorBlendAttachmentState.setSrcColorBlendFactor(
        translateColorBlendFactor(state.src_color_factor));
    mColorBlendAttachmentState.setDstColorBlendFactor(
        translateColorBlendFactor(state.dst_color_factor));
    mColorBlendAttachmentState.setSrcAlphaBlendFactor(
        translateColorBlendFactor(state.src_alpha_factor));
    mColorBlendAttachmentState.setDstAlphaBlendFactor(
        translateColorBlendFactor(state.dst_alpha_factor));
    mColorBlendAttachmentState.setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
}

std::shared_ptr<usagi::GraphicsPipeline> usagi::VulkanGraphicsPipelineCompiler::
    compile()
{
    if(!mVertexShader)
        throw std::runtime_error("Vertex shader is not set.");
    if(!mFragmentShader)
        throw std::runtime_error("Fragment shader is not set.");

    // Shader Stages
    enum ShaderStage
    {
        VERTEX,
        FRAGMENT,
        SHADER_STAGE_COUNT
    };
    vk::PipelineShaderStageCreateInfo shader_stage_create_info[
        SHADER_STAGE_COUNT];
    auto vertex_shader = createShaderModule(
        mDevice->device(), mVertexShader);
    shader_stage_create_info[VERTEX].setModule(vertex_shader.get());
    shader_stage_create_info[VERTEX].setPName("main"); // entry point name
    shader_stage_create_info[VERTEX].setStage(
        vk::ShaderStageFlagBits::eVertex);

    auto fragment_shader = createShaderModule(
        mDevice->device(), mFragmentShader);
    shader_stage_create_info[FRAGMENT].setModule(fragment_shader.get());
    shader_stage_create_info[FRAGMENT].setPName("main"); // entry point name
    shader_stage_create_info[FRAGMENT].setStage(
        vk::ShaderStageFlagBits::eFragment);

    mPipelineCreateInfo.setStageCount(SHADER_STAGE_COUNT);
    mPipelineCreateInfo.setPStages(shader_stage_create_info);

    // Vertex atttibute name map is cleared and imported into index array
    // during this process.
    reflectShaders();

    // Vertex Input
    vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info;
    vertex_input_state_create_info.setVertexBindingDescriptionCount(
        mVertexInputBindings.size());
    vertex_input_state_create_info.setPVertexBindingDescriptions(
        mVertexInputBindings.data());
    vertex_input_state_create_info.setVertexAttributeDescriptionCount(
        mVertexAttributeIndexArray.size());
    vertex_input_state_create_info.setPVertexAttributeDescriptions(
        mVertexAttributeIndexArray.data());
    mPipelineCreateInfo.setPVertexInputState(&vertex_input_state_create_info);

    // Dynamic States
    // Viewport and scissor should be set after binding the pipeline
    std::array<vk::DynamicState, 2> dynamic_state {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamic_state_create_info;
    dynamic_state_create_info.dynamicStateCount = dynamic_state.size();
    dynamic_state_create_info.pDynamicStates    = dynamic_state.data();
    mPipelineCreateInfo.pDynamicState           = &dynamic_state_create_info;

    // todo: attachments

    auto pipeline = mDevice->device().createGraphicsPipelineUnique(
        { }, mPipelineCreateInfo);

    return std::make_shared<VulkanGraphicsPipeline>(std::move(pipeline));
}

vk::Format usagi::VulkanGraphicsPipelineCompiler::
    translateSourceFormat(GpuDataFormat native_data)
{
    switch(native_data)
    {
        case GpuDataFormat::R32_SFLOAT:
            return vk::Format::eR32Sfloat;
        case GpuDataFormat::R32G32_SFLOAT:
            return vk::Format::eR32G32Sfloat;
        case GpuDataFormat::R32G32B32_SFLOAT:
            return vk::Format::eR32G32B32Sfloat;
        case GpuDataFormat::R32G32B32A32_SFLOAT:
            return vk::Format::eR32G32B32A32Sfloat;
        default:
            throw std::runtime_error("Invalid source format.");
    }
}

vk::VertexInputRate usagi::VulkanGraphicsPipelineCompiler::
    translateVertexInputRate(VertexBufferBindingInfo::InputRate rate)
{
    switch(rate)
    {
        case VertexBufferBindingInfo::InputRate::PER_VERTEX:
            return vk::VertexInputRate::eVertex;
        case VertexBufferBindingInfo::InputRate::PER_INSTANCE:
            return vk::VertexInputRate::eInstance;
        default:
            throw std::runtime_error("Invalid vertex input rate.");
    }
}

vk::PrimitiveTopology usagi::VulkanGraphicsPipelineCompiler::translateTopology(
    InputAssemblyState::PrimitiveTopology topology)
{
    switch(topology)
    {
        case InputAssemblyState::PrimitiveTopology::POINT_LIST:
            return vk::PrimitiveTopology::ePointList;
        case InputAssemblyState::PrimitiveTopology::LINE_LIST:
            return vk::PrimitiveTopology::eLineList;
        case InputAssemblyState::PrimitiveTopology::LINE_STRIP:
            return vk::PrimitiveTopology::eLineStrip;
        case InputAssemblyState::PrimitiveTopology::TRIANGLE_LIST:
            return vk::PrimitiveTopology::eTriangleList;
        case InputAssemblyState::PrimitiveTopology::TRIANGLE_STRIP:
            return vk::PrimitiveTopology::eTriangleStrip;
        case InputAssemblyState::PrimitiveTopology::TRIANGLE_FAN:
            return vk::PrimitiveTopology::eTriangleFan;
        default:
            throw std::runtime_error("Invalid primitive topology.");
    }
}

vk::CullModeFlags usagi::VulkanGraphicsPipelineCompiler::translateCullMode(
    RasterizationState::FaceCullingMode face_culling_mode)
{
    switch(face_culling_mode)
    {
        case RasterizationState::FaceCullingMode::NONE:
            return vk::CullModeFlagBits::eNone;
        case RasterizationState::FaceCullingMode::FRONT:
            return vk::CullModeFlagBits::eFront;
        case RasterizationState::FaceCullingMode::BACK:
            return vk::CullModeFlagBits::eBack;
        default:
            throw std::runtime_error("Invalid face culling mode.");
    }
}

vk::FrontFace usagi::VulkanGraphicsPipelineCompiler::translateFrontFace(
    RasterizationState::FrontFace front_face)
{
    switch(front_face)
    {
        case RasterizationState::FrontFace::CLOCKWISE:
            return vk::FrontFace::eClockwise;
        case RasterizationState::FrontFace::COUNTER_CLOCKWISE:
            return vk::FrontFace::eCounterClockwise;
        default:
            throw std::runtime_error("Invalid front face.");
    }
}

vk::PolygonMode usagi::VulkanGraphicsPipelineCompiler::translatePolygonMode(
    RasterizationState::PolygonMode polygon_mode)
{
    switch(polygon_mode)
    {
        case RasterizationState::PolygonMode::FILL:
            return vk::PolygonMode::eFill;
        case RasterizationState::PolygonMode::LINE:
            return vk::PolygonMode::eLine;
        case RasterizationState::PolygonMode::POINT:
            return vk::PolygonMode::ePoint;
        default:
            throw std::runtime_error("Invalid polygon mode.");
    }
}

vk::BlendOp usagi::VulkanGraphicsPipelineCompiler::translateBlendOp(
    ColorBlendState::BlendingOperation blending_operation)
{
    switch(blending_operation)
    {
        case ColorBlendState::BlendingOperation::ADD:
            return vk::BlendOp::eAdd;
        case ColorBlendState::BlendingOperation::SUBTRACT:
            return vk::BlendOp::eSubtract;
        case ColorBlendState::BlendingOperation::REVERSE_SUBTRACT:
            return vk::BlendOp::eReverseSubtract;
        case ColorBlendState::BlendingOperation::MIN:
            return vk::BlendOp::eMin;
        case ColorBlendState::BlendingOperation::MAX:
            return vk::BlendOp::eMax;
        default:
            throw std::runtime_error("Invalid blending operation.");
    }
}

vk::BlendFactor usagi::VulkanGraphicsPipelineCompiler::
    translateColorBlendFactor(
        ColorBlendState::BlendingFactor blending_factor)
{
    switch(blending_factor)
    {
        case ColorBlendState::BlendingFactor::ZERO:
            return vk::BlendFactor::eZero;
        case ColorBlendState::BlendingFactor::ONE:
            return vk::BlendFactor::eOne;
        case ColorBlendState::BlendingFactor::SOURCE_ALPHA:
            return vk::BlendFactor::eSrcAlpha;
        case ColorBlendState::BlendingFactor::ONE_MINUS_SOURCE_ALPHA:
            return vk::BlendFactor::eOneMinusSrcAlpha;
        default:
            throw std::runtime_error("Invalid blending factor.");
    }
}
