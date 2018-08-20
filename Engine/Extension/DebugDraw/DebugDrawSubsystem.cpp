#define DEBUG_DRAW_IMPLEMENTATION
#include "DebugDrawSubsystem.hpp"

#include <Usagi/Engine/Asset/AssetRoot.hpp>
#include <Usagi/Engine/Core/Element.hpp>
#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsPipelineCompiler.hpp>
#include <Usagi/Engine/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/Framebuffer.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuBuffer.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuCommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GraphicsCommandList.hpp>
#include <Usagi/Engine/Runtime/Runtime.hpp>
#include <Usagi/Engine/Utility/TypeCast.hpp>

#include "DebugDrawComponent.hpp"

void usagi::DebugDrawSubsystem::createPipelines(
    RenderPassCreateInfo &render_pass_info)
{
    createPointLinePipeline(render_pass_info);
}

void usagi::DebugDrawSubsystem::createPointLinePipeline(
    RenderPassCreateInfo &render_pass_info)
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    mCommandPool = gpu->createCommandPool();
    mVertexBuffer = gpu->createBuffer(GpuBufferUsage::VERTEX);

    // ~~ Point Pipelines ~~

    // Renderpass
    {
        render_pass_info.attachment_usages[0].layout
            = GpuImageLayout::COLOR_ATTACHMENT;
        render_pass_info.attachment_usages[1].layout
            = GpuImageLayout::DEPTH_STENCIL_ATTACHMENT;
        mRenderPass = gpu->createRenderPass(render_pass_info);
        compiler->setRenderPass(mRenderPass);
    }
    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->find<SpirvBinary>("dd:shaders/pointline.vert")
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->find<SpirvBinary>("dd:shaders/pointline.frag")
        );
    }
    // Vertex Inputs
    {
        compiler->setVertexBufferBinding(0, sizeof(dd::DrawVertex));

        std::size_t offset = 0;

        compiler->setVertexAttribute(
            "in_Position", 0,
            static_cast<uint32_t>(offset), GpuBufferFormat::R32G32B32_SFLOAT
        );
        offset += sizeof(float) * 3;

        compiler->setVertexAttribute(
            "in_ColorPointSize", 0,
            static_cast<uint32_t>(offset), GpuBufferFormat::R32G32B32A32_SFLOAT
        );
    }
    compiler->iaSetPrimitiveTopology(PrimitiveTopology::POINT_LIST);
    compiler->rsSetPolygonmode(PolygonMode::POINT);
    compiler->rsSetFaceCullingMode(FaceCullingMode::NONE);
    compiler->omSetDepthEnabled(true);
    compiler->omSetColorBlendEnabled(false);

    mPointDepthEnabledPipeline = compiler->compile();
    compiler->omSetDepthEnabled(false);

    // Create child pipeline
    mPointDepthDisabledPipeline = compiler->compile();

    // ~~ Line Pipelines ~~

    compiler->iaSetPrimitiveTopology(PrimitiveTopology::LINE_LIST);
    compiler->rsSetPolygonmode(PolygonMode::LINE);
    mLineDepthDisabledPipeline = compiler->compile();

    compiler->omSetDepthEnabled(true);
    mLineDepthEnabledPipeline = compiler->compile();
}

usagi::DebugDrawSubsystem::DebugDrawSubsystem(Game *game)
    : mGame(game)
{
    dd::initialize(&mContext, this);
}

usagi::DebugDrawSubsystem::~DebugDrawSubsystem()
{
    dd::shutdown(mContext);
    mContext = nullptr;
}

void usagi::DebugDrawSubsystem::update(
    const TimeDuration &dt,
    const std::shared_ptr<Framebuffer> framebuffer,
    const CommandListSink &cmd_out)
{
    for(auto &&e : mRegistry)
    {
        e->getComponent<DebugDrawComponent>()->draw(mContext);
    }

    mCurrentCmdList = mCommandPool->allocateGraphicsCommandList();
    mCurrentCmdList->beginRecording();
    mCurrentCmdList->beginRendering(mRenderPass, framebuffer);
    mCurrentCmdList->setViewport(
        0, { 0, 0 }, framebuffer->size().cast<float>());
    mCurrentCmdList->setScissor(0, { 0, 0 }, framebuffer->size());
    dd::flush(mContext);
    mCurrentCmdList->endRendering();
    mCurrentCmdList->endRecording();
    cmd_out(std::move(mCurrentCmdList));
}

bool usagi::DebugDrawSubsystem::processable(Element *element)
{
    return is_instance_of<DebugDrawComponent>(element);
}

void usagi::DebugDrawSubsystem::updateRegistry(Element *element)
{
    if(element->hasComponent<DebugDrawComponent>())
        mRegistry.insert(element);
    else
        mRegistry.erase(element);
}

void usagi::DebugDrawSubsystem::drawPointList(
    const dd::DrawVertex *points,
    const int count,
    const bool depth_enabled)
{
    mVertexBuffer->allocate(count * sizeof(dd::DrawVertex));
    {
        const auto mem = mVertexBuffer->mappedMemory();
        memcpy(mem, points, mVertexBuffer->size());
        mVertexBuffer->flush();
    }
    mCurrentCmdList->bindPipeline(
        depth_enabled
            ? mPointDepthEnabledPipeline
            : mPointDepthDisabledPipeline
    );
    // dd uses column-major matrices
    // todo

    Matrix4f m;
    m << -1.04781950f, 3.54236340e-06f, -0.767837644f, -3.74200988f,
        -0.535599470f, 1.47611356f, 0.730905473f, -3.28316879f,
        -0.503842056f, -0.523264706f, 0.687558949f, 8.64296627f,
        -0.503741324f, -0.523160100f, 0.687421501f, 8.84121895f;

    mCurrentCmdList->setConstant(
        ShaderStage::VERTEX, "u_MvpMatrix",
        m.data(), 16 * sizeof(float)
    );
    mCurrentCmdList->bindVertexBuffer(0, mVertexBuffer.get(), 0);
    mCurrentCmdList->drawInstanced(count, 1, 0, 0);
    mVertexBuffer->release();
}

void usagi::DebugDrawSubsystem::drawLineList(
    const dd::DrawVertex *lines,
    const int count,
    const bool depth_enabled)
{
    mVertexBuffer->allocate(count * sizeof(dd::DrawVertex));
    {
        const auto mem = mVertexBuffer->mappedMemory();
        memcpy(mem, lines, mVertexBuffer->size());
        mVertexBuffer->flush();
    }
    mCurrentCmdList->bindPipeline(
        depth_enabled
        ? mLineDepthEnabledPipeline
        : mLineDepthDisabledPipeline
    );
    mCurrentCmdList->setLineWidth(1.f);
    Matrix4f m;
    m << -1.04781950f, 3.54236340e-06f, -0.767837644f, -3.74200988f,
        -0.535599470f, 1.47611356f, 0.730905473f, -3.28316879f,
        -0.503842056f, -0.523264706f, 0.687558949f, 8.64296627f,
        -0.503741324f, -0.523160100f, 0.687421501f, 8.84121895f;

    mCurrentCmdList->setConstant(
        ShaderStage::VERTEX, "u_MvpMatrix",
        m.data(), 16 * sizeof(float)
    );
    mCurrentCmdList->bindVertexBuffer(0, mVertexBuffer.get(), 0);
    mCurrentCmdList->drawInstanced(count, 1, 0, 0);
    mVertexBuffer->release();
}

void usagi::DebugDrawSubsystem::drawGlyphList(
    const dd::DrawVertex *glyphs,
    int count,
    dd::GlyphTextureHandle glyph_tex)
{
    // todo
}
