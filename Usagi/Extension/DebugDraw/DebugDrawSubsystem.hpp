﻿#pragma once

#include <set>

#include <Usagi/Core/Math.hpp>
#include <Usagi/Runtime/Graphics/RenderableSubsystem.hpp>

#include "DebugDraw.hpp"

namespace usagi
{
class GpuImage;
class GpuSampler;
class RenderPass;
struct RenderPassCreateInfo;
class Game;
class GraphicsPipeline;
class GpuCommandPool;
class GpuBuffer;

class DebugDrawSubsystem
    : public RenderableSubsystem
    , public dd::RenderInterface
{
    Game *mGame = nullptr;
    dd::ContextHandle mContext = nullptr;
    std::set<Element*> mRegistry;

    std::shared_ptr<RenderPass> mRenderPass;
    std::shared_ptr<GraphicsPipeline> mPointDepthEnabledPipeline;
    std::shared_ptr<GraphicsPipeline> mPointDepthDisabledPipeline;
    std::shared_ptr<GraphicsPipeline> mLineDepthEnabledPipeline;
    std::shared_ptr<GraphicsPipeline> mLineDepthDisabledPipeline;
    std::shared_ptr<GraphicsPipeline> mTextPipeline;
    std::shared_ptr<GpuImage> mFontTexture;
    std::shared_ptr<GpuSampler> mFontSampler;
    std::shared_ptr<GpuCommandPool> mCommandPool;
    std::shared_ptr<GpuBuffer> mVertexBuffer;
    mutable std::shared_ptr<GraphicsCommandList> mCurrentCmdList;
    mutable Vector2f mDisplaySize;
    /**
     * \brief The matrix for transforming from world coordinates to
     * clip space.
     */
    Projective3f mWorldToNDC;

    void createPointLinePipeline();
    void createTextPipeline();

public:
    explicit DebugDrawSubsystem(Game *game);
    ~DebugDrawSubsystem();

    void createPipelines(RenderPassCreateInfo &render_pass_info);

    void setWorldToNDC(const Projective3f &mat);
    void update(const TimeDuration &dt) override;
    void render(
        const TimeDuration &dt,
        std::shared_ptr<Framebuffer> framebuffer,
        const CommandListSink &cmd_out) const override;
    bool processable(Element *element) override;
    void updateRegistry(Element *element) override;

    dd::GlyphTextureHandle createGlyphTexture(
        int width,
        int height,
        const void *pixels) override;
    void destroyGlyphTexture(dd::GlyphTextureHandle) override;

    void drawPointList(
        const dd::DrawVertex *points,
        int count,
        bool depth_enabled) override;
    void drawLineList(
        const dd::DrawVertex *lines,
        int count,
        bool depth_enabled) override;
    void drawGlyphList(
        const dd::DrawVertex *glyphs,
        int count,
        dd::GlyphTextureHandle glyph_tex) override;
};
}