#include "SortedSpriteRenderingSubsystem.hpp"

#include <array>

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
#include <Usagi/Runtime/Graphics/GpuImageView.hpp>
#include <Usagi/Runtime/Graphics/GpuSampler.hpp>
#include <Usagi/Runtime/Graphics/GpuSamplerCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>
#include <Usagi/Runtime/Graphics/GraphicsPipelineCompiler.hpp>
#include <Usagi/Runtime/Graphics/Shader/SpirvBinary.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Transform/TransformComponent.hpp>

#include "SpriteComponent.hpp"

namespace usagi::moeloop
{
struct SortedSpriteRenderingSubsystem::SpriteIn
{
    Vector3f pos;
    Vector2f uv[SpriteComponent::NUM_LAYERS];
};

void SortedSpriteRenderingSubsystem::createBuffers()
{
    auto gpu = mGame->runtime()->gpu();

    mVertexBuffer = gpu->createBuffer(GpuBufferUsage::VERTEX);
    mIndexBuffer = gpu->createBuffer(GpuBufferUsage::INDEX);

    mIndexBuffer->allocate(sizeof(uint16_t) * 4);
    // draw a quad using triangle strip. refer to update() for vertex order.
    const auto indices = mIndexBuffer->mappedMemory<uint16_t>();
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;
    indices[3] = 3;
    mIndexBuffer->flush();
}

SortedSpriteRenderingSubsystem::SortedSpriteRenderingSubsystem(
    Game *game,
    CompareFunc compare_func)
    : mGame(game)
    , mCompareFunc(std::move(compare_func))
{
    createBuffers();
    auto gpu = mGame->runtime()->gpu();
    mCommandPool = gpu->createCommandPool();
}

void SortedSpriteRenderingSubsystem::createRenderTarget(
    RenderTargetDescriptor &descriptor)
{
    descriptor.sharedColorTarget("mlsprite");
    // descriptor.depthTarget();
    mRenderTarget = descriptor.finish();
}

void SortedSpriteRenderingSubsystem::createPipelines()
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->res<SpirvAssetConverter>(
                "moeloop:shaders/sprite.vert", ShaderStage::VERTEX)
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->res<SpirvAssetConverter>(
                "moeloop:shaders/sprite.frag", ShaderStage::FRAGMENT)
        );
    }
    // Vertex Inputs
    {
        compiler->setVertexBufferBinding(0, sizeof(SpriteIn));

        compiler->setVertexAttribute(
            "in_pos", 0,
            offsetof(SpriteIn, pos), GpuBufferFormat::R32G32B32_SFLOAT
        );
        compiler->setVertexAttribute(
            "in_uv0", 0,
            offsetof(SpriteIn, uv[0]), GpuBufferFormat::R32G32_SFLOAT
        );
        compiler->setVertexAttribute(
            "in_uv1", 0,
            offsetof(SpriteIn, uv[1]), GpuBufferFormat::R32G32_SFLOAT
        );

        compiler->iaSetPrimitiveTopology(PrimitiveTopology::TRIANGLE_STRIP);
    }
    // Rasterization
    {
        RasterizationState state;
        state.face_culling_mode = FaceCullingMode::NONE;
        compiler->setRasterizationState(state);
    }
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
    compiler->setRenderPass(mRenderTarget->renderPass());
    // Depth
    {
        compiler->omSetDepthEnabled(false);
    }

    mPipeline = compiler->compile();

    mSampler = gpu->createSampler({ });
}

void SortedSpriteRenderingSubsystem::update(const Clock &clock)
{
    mSortedElements.clear();
    mSortedElements.reserve(mRegistry.size());
    for(auto i = mRegistry.begin(); i != mRegistry.end(); ++i)
    {
        mSortedElements.push_back(i);
    }
    if(mSortedElements.empty()) return;

    std::sort(mSortedElements.begin(), mSortedElements.end(),
        [&](auto &&l, auto &&r) {
            return mCompareFunc(
                std::get<TransformComponent*>(l->second),
                std::get<SpriteComponent*>(l->second),
                std::get<TransformComponent*>(r->second),
                std::get<SpriteComponent*>(r->second)
            );
        });

    mVertexBuffer->allocate(mSortedElements.size() * sizeof(SpriteIn) * 4);

    const auto verts = mVertexBuffer->mappedMemory<SpriteIn>();
    std::size_t vert_idx = 0;
    for(auto &&e : mSortedElements)
    {
        const auto s = std::get<SpriteComponent*>(e->second);
        if(!s->show)
        {
            vert_idx += 4;
            continue;
        }

        static_assert(SpriteComponent::NUM_LAYERS > 0);

        std::array<AlignedBox2f, SpriteComponent::NUM_LAYERS> box;
        for(std::size_t i = 0; i < box.size(); ++i)
        {
            if(s->layers[i].texture)
                box[i] = s->layers[i].quad();
        }
        auto bbox = box[0];
        for(std::size_t i = 1; i < box.size(); ++i)
        {
            if(s->layers[i].texture)
                bbox = bbox.merged(box[i]);
        }
        std::array<AlignedBox2f, SpriteComponent::NUM_LAYERS> uv;
        for(std::size_t i = 0; i < uv.size(); ++i)
        {
            if(s->layers[i].texture)
                uv[i] = s->layers[i].toUV(bbox);
        }

        const std::array<AlignedBox2f::CornerType, 4> corners = {
            AlignedBox2f::TopLeft,
            AlignedBox2f::TopRight,
            AlignedBox2f::BottomLeft,
            AlignedBox2f::BottomRight
        };

        const auto converter = [](const Vector2f &v) {
            return Vector3f { v.x(), 0, -v.y() };
        };
        for(std::size_t i = 0; i < corners.size(); ++i, ++vert_idx)
        {
            verts[vert_idx].pos = converter(bbox.corner(corners[i]));
            for(std::size_t j = 0; j < uv.size(); ++j)
                verts[vert_idx].uv[j] = uv[j].corner(corners[i]);
        }
    }

    mVertexBuffer->flush();
}

std::shared_ptr<GraphicsCommandList> SortedSpriteRenderingSubsystem::render(
    const Clock &clock)
{
    if(mSortedElements.empty()) return { };

    auto cmd_list = mCommandPool->allocateGraphicsCommandList();
    const auto framebuffer = mRenderTarget->createFramebuffer();

    cmd_list->beginRecording();
    cmd_list->beginRendering(mRenderTarget->renderPass(), framebuffer);
    cmd_list->setViewport(
        0, { 0, 0 }, framebuffer->size().cast<float>());
    cmd_list->setScissor(0, { 0, 0 }, framebuffer->size());
    cmd_list->bindPipeline(mPipeline);
    cmd_list->bindVertexBuffer(0, mVertexBuffer, 0);
    cmd_list->bindIndexBuffer(mIndexBuffer, 0, GraphicsIndexType::UINT16);
    cmd_list->bindResourceSet(0, { mSampler });

    for(std::size_t i = 0; i < mSortedElements.size(); ++i)
    {
        const auto s = std::get<SpriteComponent*>(mSortedElements[i]->second);
        // ignore hidden sprites
        if(!s->show) continue;
        if(!s->layers[0].texture && !s->layers[1].texture) continue;
        const auto t = std::get<TransformComponent*>(mSortedElements[i]->second);

        // todo more elements?

        const auto fallback = mGame->runtime()->gpu()->fallbackTexture();
        cmd_list->bindResourceSet(1, {
            s->layers[0].texture ?
                s->layers[0].texture->baseView() : fallback->baseView(),
            s->layers[1].texture ?
                s->layers[1].texture->baseView() : fallback->baseView(),
        });

        cmd_list->setConstant(ShaderStage::VERTEX, "mvp_matrix",
            (mWorldToNdcFunc() * t->localToWorld()).data(), sizeof(float) * 16);

        cmd_list->setConstant(ShaderStage::FRAGMENT, "layer0",
            &s->layers[0].uv_rect, sizeof(float) * 5);
        cmd_list->setConstant(ShaderStage::FRAGMENT, "layer1",
            &s->layers[1].uv_rect, sizeof(float) * 5);

        cmd_list->drawIndexedInstanced(
            4, 1, 0, static_cast<std::uint32_t>(4 * i), 0);
    }

    cmd_list->endRendering();
    cmd_list->endRecording();

    return std::move(cmd_list);
}
}
