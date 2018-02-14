#include <cstring>

#include <Usagi/Engine/Runtime/Graphics/Device/Device.hpp>
#include <Usagi/Engine/Runtime/Graphics/SpirvShader.hpp>
#include <Usagi/Engine/Runtime/Graphics/Environment.hpp>
#include <Usagi/Engine/Runtime/Graphics/Device/SwapChain.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/CommandList.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/Pipeline.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceManager.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/DynamicBuffer.hpp>
#include <Usagi/Engine/Runtime/Graphics/Workload/PipelineCreateInfo.hpp>

#include "VulkanTriangle.hpp"

using namespace yuki::graphics;

yuki::VulkanTriangle::VulkanTriangle(Environment *env)
    : mEnv { env }
{
    {
        PipelineCreateInfo graphics_pipeline_create_info;
        graphics_pipeline_create_info.vertex_input.bindings.push_back({
            0,
            sizeof(VertexData)
        });
        graphics_pipeline_create_info.vertex_input.attributes.push_back({
            0,
            0,
            DataFormat::R32G32B32A32_SFLOAT,
            0
        });
        graphics_pipeline_create_info.vertex_input.attributes.push_back({
            1,
            0,
            DataFormat::R32G32B32A32_SFLOAT,
            4 * sizeof(float)
        });
        graphics_pipeline_create_info.vertex_shader = SpirvShader::readFromFile(
            R"(E:\Projects\IntroductionToVulkan\Project\Tutorial04\Data04\vert.spv)");
        graphics_pipeline_create_info.fragment_shader = SpirvShader::readFromFile(
            R"(E:\Projects\IntroductionToVulkan\Project\Tutorial04\Data04\frag.spv)");
        graphics_pipeline_create_info.input_assembly.topology = InputAssembleState::
            PrimitiveTopology::TRIANGLE_STRIP;
        graphics_pipeline_create_info.rasterization.face_culling_mode = RasterizationState
            ::FaceCullingMode::BACK;
        graphics_pipeline_create_info.rasterization.front_face = RasterizationState::
            FrontFace::COUNTER_CLOCKWISE;
        graphics_pipeline_create_info.rasterization.polygon_mode = RasterizationState::
            PolygonMode::FILL;
        graphics_pipeline_create_info.depth_stencil.enable_depth_test = false;
        graphics_pipeline_create_info.color_blend.enable = false;
        // todo: Renderable shouldn't know the actual render targets, as they are managed on the application layer
        graphics_pipeline_create_info.attachment_usages.push_back({
            env->swap_chain->getNativeImageFormat(),
            ImageLayout::UNDEFINED,
            ImageLayout::PRESENT_SRC
        });
        mPipeline = env->graphics_device->createPipeline(graphics_pipeline_create_info);
    }

    mVertexBuffer = env->resource_manager->createDynamicBuffer(
        sizeof(VertexData) * 4,
        DynamicBuffer::UsageFlag::STREAM
    );

    // todo move to another demo
    //{
    //    SamplerCreateInfo sampler_create_info;
    //    sampler_create_info.address_mode_u = SamplerCreateInfo::AddressMode::
    //        CLAMP_TO_EDGE;
    //    sampler_create_info.address_mode_v = SamplerCreateInfo::AddressMode::
    //        CLAMP_TO_EDGE;
    //    sampler_create_info.magnify_filter = SamplerCreateInfo::Filter::NEAREST;
    //    sampler_create_info.minify_filter = SamplerCreateInfo::Filter::LINEAR;
    //    mSampler = env->graphics_device->createSampler(sampler_create_info);
    //}
}

yuki::VulkanTriangle::~VulkanTriangle()
{
}

void yuki::VulkanTriangle::update(double delta_time)
{
    mCounter1 += delta_time;

    float a = abs(sin(mCounter1));
    float b = abs(sin(mCounter1 + 0.25));
    float c = abs(sin(mCounter1 + 0.5));
    float d = abs(sin(mCounter1 + 0.75));

    VertexData vertex_data[]
    {
        { -0.7f, -0.7f, 0.0f, 1.0f, a, b, c, 0.0f },
        { -0.7f, 0.7f, 0.0f, 1.0f, c, a, b, 0.0f },
        { 0.7f, -0.7f, 0.0f, 1.0f, b, c, a, 0.0f },
        { 0.7f, 0.7f, 0.0f, 1.0f, c, a, c, 0.0f }
    };

    if(const auto mem = mVertexBuffer->map(0, sizeof(vertex_data)))
    {
        memcpy(mem, vertex_data, sizeof(vertex_data));
        mVertexBuffer->unmap();
    }
}

void yuki::VulkanTriangle::populateCommandList(CommandList *command_list)
{
    command_list->bindPipeline(mPipeline.get());
    command_list->setScissor(0, 0, 1280, 720);
    command_list->setViewport(0, 0, 1280, 720);
    command_list->bindVertexBuffer(0, mVertexBuffer.get());
    command_list->draw(4, 1, 0, 0);
}
