#include "DebugDrawSystem.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/SpirvAssetConverter.hpp>
#include <Usagi/Game/Game.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTarget.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetDescriptor.hpp>
#include <Usagi/Runtime/Graphics/Framebuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuBuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuCommandPool.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GpuImageView.hpp>
#include <Usagi/Runtime/Graphics/GpuSampler.hpp>
#include <Usagi/Runtime/Graphics/GpuSamplerCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>
#include <Usagi/Runtime/Graphics/GraphicsPipelineCompiler.hpp>
#include <Usagi/Runtime/Runtime.hpp>

void usagi::DebugDrawSystem::createRenderTarget(
    RenderTargetDescriptor &descriptor)
{
    descriptor.sharedColorTarget("debugdraw");
    descriptor.depthTarget();
    mRenderTarget = descriptor.finish();
}

void usagi::DebugDrawSystem::createPipelines()
{
    auto gpu = mGame->runtime()->gpu();
    mCommandPool = gpu->createCommandPool();
    mVertexBuffer = gpu->createBuffer(GpuBufferUsage::VERTEX);

    createPointLinePipeline();
    createTextPipeline();
}

void usagi::DebugDrawSystem::createPointLinePipeline()
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    // ~~ Point Pipelines ~~

    compiler->setRenderPass(mRenderTarget->renderPass());
    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->res<SpirvAssetConverter>(
                "dd:shaders/pointline.vert", ShaderStage::VERTEX)
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->res<SpirvAssetConverter>(
                "dd:shaders/pointline.frag", ShaderStage::FRAGMENT)
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
    compiler->rsSetPolygonMode(PolygonMode::POINT);
    compiler->rsSetFaceCullingMode(FaceCullingMode::NONE);
    compiler->omSetDepthEnabled(true);
    compiler->omSetColorBlendEnabled(false);

    mPointDepthEnabledPipeline = compiler->compile();

    // Create child pipelines

    compiler->omSetDepthEnabled(false);
    mPointDepthDisabledPipeline = compiler->compile();

    // ~~ Line Pipelines ~~

    compiler->iaSetPrimitiveTopology(PrimitiveTopology::LINE_LIST);
    compiler->rsSetPolygonMode(PolygonMode::LINE);
    mLineDepthDisabledPipeline = compiler->compile();

    compiler->omSetDepthEnabled(true);
    mLineDepthEnabledPipeline = compiler->compile();
}

void usagi::DebugDrawSystem::createTextPipeline()
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    compiler->setRenderPass(mRenderTarget->renderPass());
    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->res<SpirvAssetConverter>(
                "dd:shaders/text.vert", ShaderStage::VERTEX)
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->res<SpirvAssetConverter>(
                "dd:shaders/text.frag", ShaderStage::FRAGMENT)
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
    compiler->rsSetPolygonMode(PolygonMode::FILL);
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

usagi::DebugDrawSystem::DebugDrawSystem(Game *game)
    : mGame(game)
{
    dd::initialize(&mContext, this);
}

usagi::DebugDrawSystem::~DebugDrawSystem()
{
    dd::shutdown(mContext);
    mContext = nullptr;
}

void usagi::DebugDrawSystem::update(const Clock &clock)
{
    for(auto &&e : mRegistry)
    {
        std::get<DebugDrawComponent*>(e.second)->draw(mContext);
    }
}

std::shared_ptr<usagi::GraphicsCommandList> usagi::DebugDrawSystem::render(
    const Clock &clock)
{
    const auto framebuffer = mRenderTarget->createFramebuffer();

    mCurrentCmdList = mCommandPool->allocateGraphicsCommandList();
    mCurrentCmdList->beginRecording();
    mCurrentCmdList->beginRendering(mRenderTarget->renderPass(), framebuffer);
    mCurrentCmdList->setViewport(
        0, { 0, 0 }, framebuffer->size().cast<float>());
    mCurrentCmdList->setScissor(0, { 0, 0 }, framebuffer->size());
    dd::flush(mContext);
    mCurrentCmdList->endRendering();
    mCurrentCmdList->endRecording();

    return std::move(mCurrentCmdList);
}

dd::GlyphTextureHandle usagi::DebugDrawSystem::createGlyphTexture(
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

void usagi::DebugDrawSystem::destroyGlyphTexture(dd::GlyphTextureHandle)
{
    mFontTexture.reset();
    mFontSampler.reset();
}

void usagi::DebugDrawSystem::drawPointList(
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
    mCurrentCmdList->setConstant(
        ShaderStage::VERTEX, "u_MvpMatrix",
        mWorldToNdcFunc().data(), 16 * sizeof(float)
    );
    mCurrentCmdList->bindVertexBuffer(0, mVertexBuffer, 0);
    mCurrentCmdList->drawInstanced(count, 1, 0, 0);
    mVertexBuffer->release();
}

void usagi::DebugDrawSystem::drawLineList(
    const dd::DrawVertex *lines,
    const int count,
    const bool depth_enabled)
{
    mVertexBuffer->allocate(count * sizeof(dd::DrawVertex));
    {
        // todo add a method
        const auto mem = mVertexBuffer->mappedMemory();
        memcpy(mem, lines, mVertexBuffer->size());
        mVertexBuffer->flush();
    }
    mCurrentCmdList->bindPipeline(
        depth_enabled
        ? mLineDepthEnabledPipeline
        : mLineDepthDisabledPipeline
    );
    // todo add a setting
    mCurrentCmdList->setLineWidth(1.f);
    mCurrentCmdList->setConstant(
        ShaderStage::VERTEX, "u_MvpMatrix",
        mWorldToNdcFunc().data(), 16 * sizeof(float)
    );
    mCurrentCmdList->bindVertexBuffer(0, mVertexBuffer, 0);
    mCurrentCmdList->drawInstanced(count, 1, 0, 0);
    mVertexBuffer->release();
}

void usagi::DebugDrawSystem::drawGlyphList(
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
        mWindowSizeFunc().data(), 2 * sizeof(float)
    );
    mCurrentCmdList->bindResourceSet(0, {
        mFontSampler,
        reinterpret_cast<GpuImageView*>(glyph_tex)->shared_from_this()
    });
    mCurrentCmdList->bindVertexBuffer(0, mVertexBuffer, 0);
    mCurrentCmdList->drawInstanced(count, 1, 0, 0);
    mVertexBuffer->release();
}
