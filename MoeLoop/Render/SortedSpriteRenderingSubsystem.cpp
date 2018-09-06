#include "SortedSpriteRenderingSubsystem.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/SpirvAssetConverter.hpp>
#include <Usagi/Game/Game.hpp>
#include <Usagi/Runtime/Graphics/Framebuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuBuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GpuImageView.hpp>
#include <Usagi/Runtime/Graphics/GpuSampler.hpp>
#include <Usagi/Runtime/Graphics/GpuSamplerCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>
#include <Usagi/Runtime/Graphics/GraphicsPipelineCompiler.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/Shader/SpirvBinary.hpp>
#include <Usagi/Runtime/Runtime.hpp>

namespace usagi::moeloop
{
struct SortedSpriteRenderingSubsystem::SpriteIn
{
    Vector3f pos;
    Vector2f uv;
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

void SortedSpriteRenderingSubsystem::createPipeline(
    RenderPassCreateInfo render_pass_info)
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
            "in_uv", 0,
            offsetof(SpriteIn, uv), GpuBufferFormat::R32G32_SFLOAT
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
    // Render Pass
    {
        render_pass_info.attachment_usages[0].layout =
            GpuImageLayout::COLOR_ATTACHMENT;
        // todo unused. rework attachment negotiation
        render_pass_info.attachment_usages[1].layout =
            GpuImageLayout::DEPTH_STENCIL_ATTACHMENT;
        mRenderPass = gpu->createRenderPass(render_pass_info);
        compiler->setRenderPass(mRenderPass);
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
        const auto sprite = std::get<SpriteComponent*>(e->second);
        if(!sprite->texture || !sprite->show)
        {
            vert_idx += 4;
            continue;
        }

        // size of the texture rect
        const Vector2f size =
            sprite->uv_rect.sizes()
            .cwiseProduct(sprite->texture->size().cast<float>());
        const auto min = sprite->uv_rect.min();
        const auto max = sprite->uv_rect.max();
        // top-left (default origin)
        verts[vert_idx].pos = { 0, 0, 0 };
        verts[vert_idx++].uv = min;
        // top-right
        verts[vert_idx].pos = { size.x(), 0, 0 };
        verts[vert_idx++].uv = { max.x(), min.y() };
        // bottom-left
        verts[vert_idx].pos = { 0, 0, -size.y() };
        verts[vert_idx++].uv = { min.x(), max.y() };
        // bottom-right
        verts[vert_idx].pos = { size.x(), 0, -size.y() };
        verts[vert_idx++].uv = max;
    }

    mVertexBuffer->flush();
}

void SortedSpriteRenderingSubsystem::render(
    const Clock &clock,
    std::shared_ptr<Framebuffer> framebuffer,
    const CommandListSink &cmd_out) const
{
    if(mSortedElements.empty()) return;

    auto cmd_list = mCommandPool->allocateGraphicsCommandList();

    cmd_list->beginRecording();
    cmd_list->beginRendering(mRenderPass, framebuffer);
    cmd_list->setViewport(0, { 0, 0 }, framebuffer->size().cast<float>());
    cmd_list->setScissor(0, { 0, 0 }, framebuffer->size());
    cmd_list->bindPipeline(mPipeline);
    cmd_list->bindResourceSet(0, { mSampler });
    cmd_list->bindVertexBuffer(0, mVertexBuffer, 0);
    cmd_list->bindIndexBuffer(mIndexBuffer, 0, GraphicsIndexType::UINT16);

    for(std::size_t i = 0; i < mSortedElements.size(); ++i)
    {
        const auto s = std::get<SpriteComponent*>(mSortedElements[i]->second);
        // ignore unloaded / hidden sprites
        if(!s->texture || !s->show) continue;
        const auto t = std::get<TransformComponent*>(mSortedElements[i]->second);
        cmd_list->bindResourceSet(1, { s->texture->baseView() });
        cmd_list->setConstant(ShaderStage::VERTEX, "mvp_matrix",
            (mWorldToNDC * t->localToWorld()).data(), sizeof(float) * 16);
        cmd_list->setConstant(ShaderStage::FRAGMENT, "fade",
            &s->fade, sizeof(s->fade));
        cmd_list->drawIndexedInstanced(
            4, 1, 0, static_cast<std::uint32_t>(4 * i), 0);
    }

    cmd_list->endRendering();
    cmd_list->endRecording();

    cmd_out(std::move(cmd_list));
}
}
