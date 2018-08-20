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
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImage.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageView.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuSampler.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuSamplerCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GraphicsCommandList.hpp>
#include <Usagi/Engine/Runtime/Runtime.hpp>
#include <Usagi/Engine/Utility/TypeCast.hpp>

#include "DebugDrawComponent.hpp"

void usagi::DebugDrawSubsystem::createPipelines(
    RenderPassCreateInfo &render_pass_info)
{
    render_pass_info.attachment_usages[0].layout
        = GpuImageLayout::COLOR_ATTACHMENT;
    render_pass_info.attachment_usages[1].layout
        = GpuImageLayout::DEPTH_STENCIL_ATTACHMENT;

    auto gpu = mGame->runtime()->gpu();
    mRenderPass = gpu->createRenderPass(render_pass_info);
    mCommandPool = gpu->createCommandPool();
    mVertexBuffer = gpu->createBuffer(GpuBufferUsage::VERTEX);

    createPointLinePipeline();
    createTextPipeline();
}

void usagi::DebugDrawSubsystem::createPointLinePipeline()
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    // ~~ Point Pipelines ~~

    compiler->setRenderPass(mRenderPass);
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

    // Create child pipelines

    compiler->omSetDepthEnabled(false);
    mPointDepthDisabledPipeline = compiler->compile();

    // ~~ Line Pipelines ~~

    compiler->iaSetPrimitiveTopology(PrimitiveTopology::LINE_LIST);
    compiler->rsSetPolygonmode(PolygonMode::LINE);
    mLineDepthDisabledPipeline = compiler->compile();

    compiler->omSetDepthEnabled(true);
    mLineDepthEnabledPipeline = compiler->compile();
}

void usagi::DebugDrawSubsystem::createTextPipeline()
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    compiler->setRenderPass(mRenderPass);
    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->find<SpirvBinary>("dd:shaders/text.vert")
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->find<SpirvBinary>("dd:shaders/text.frag")
        );
    }
    // Vertex Inputs
    {
        compiler->setVertexBufferBinding(0, sizeof(dd::DrawVertex));

        std::size_t offset = 0;

        compiler->setVertexAttribute(
            "in_Position", 0,
            static_cast<uint32_t>(offset), GpuBufferFormat::R32G32_SFLOAT
        );
        offset += sizeof(float) * 2;

        compiler->setVertexAttribute(
            "in_TexCoords", 0,
            static_cast<uint32_t>(offset), GpuBufferFormat::R32G32_SFLOAT
        );
        offset += sizeof(float) * 2;

        compiler->setVertexAttribute(
            "in_Color", 0,
            static_cast<uint32_t>(offset), GpuBufferFormat::R32G32B32_SFLOAT
        );
    }
    compiler->iaSetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
    compiler->rsSetPolygonmode(PolygonMode::FILL);
    compiler->rsSetFaceCullingMode(FaceCullingMode::NONE);
    compiler->omSetDepthEnabled(false);

    // Blending
    {
        ColorBlendState state;
        state.enable = true;
        state.setBlendingFactor(
            BlendingFactor::SOURCE_ALPHA,
            BlendingFactor::ONE_MINUS_SOURCE_ALPHA);
        state.setBlendingOperation(BlendingOperation::ADD);
        compiler->setColorBlendState(state);
    }

    mTextPipeline = compiler->compile();
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

void usagi::DebugDrawSubsystem::update(const TimeDuration &dt)
{
    for(auto &&e : mRegistry)
    {
        e->getComponent<DebugDrawComponent>()->draw(mContext);
    }
}

void usagi::DebugDrawSubsystem::render(
    const TimeDuration &dt,
    const std::shared_ptr<Framebuffer> framebuffer,
    const CommandListSink &cmd_out) const
{
    mDisplaySize = framebuffer->size().cast<float>();

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

dd::GlyphTextureHandle usagi::DebugDrawSubsystem::createGlyphTexture(
    const int width,
    const int height,
    const void *pixels)
{
    assert(!mFontTexture);
    assert(!mFontSampler);

    auto gpu = mGame->runtime()->gpu();
    {
        GpuImageCreateInfo info;
        info.format = GpuBufferFormat::R8_UNORM;
        info.size = { width, height };
        info.usage = GpuImageUsage::SAMPLED;
        mFontTexture = gpu->createImage(info);
        mFontTexture->upload(pixels, width * height * sizeof(char));
    }
    {
        GpuSamplerCreateInfo info;
        info.min_filter = GpuFilter::LINEAR;
        info.mag_filter = GpuFilter::NEAREST;
        info.addressing_mode_u = GpuSamplerAddressMode::REPEAT;
        info.addressing_mode_v = GpuSamplerAddressMode::REPEAT;
        mFontSampler = gpu->createSampler(info);
    }
    return reinterpret_cast<dd::GlyphTextureHandle>(
        mFontTexture->baseView().get());
}

void usagi::DebugDrawSubsystem::destroyGlyphTexture(dd::GlyphTextureHandle)
{
    mFontTexture.reset();
    mFontSampler.reset();
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
    const int count,
    const dd::GlyphTextureHandle glyph_tex)
{
    mVertexBuffer->allocate(count * sizeof(dd::DrawVertex));
    {
        const auto mem = mVertexBuffer->mappedMemory();
        memcpy(mem, glyphs, mVertexBuffer->size());
        mVertexBuffer->flush();
    }
    mCurrentCmdList->bindPipeline(mTextPipeline);
    mCurrentCmdList->setConstant(
        ShaderStage::VERTEX, "u_screenDimensions",
        mDisplaySize.data(), 2 * sizeof(float)
    );
    mCurrentCmdList->bindResourceSet(0, {
        mFontSampler,
        reinterpret_cast<GpuImageView*>(glyph_tex)->shared_from_this()
    });
    mCurrentCmdList->bindVertexBuffer(0, mVertexBuffer.get(), 0);
    mCurrentCmdList->drawInstanced(count, 1, 0, 0);
    mVertexBuffer->release();
}
