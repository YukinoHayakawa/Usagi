#pragma once

#include <Usagi/Graphics/ProjectiveRenderingSubsystem.hpp>
#include <Usagi/Graphics/OverlayRenderingSubsystem.hpp>
#include <Usagi/Game/CollectionSubsystem.hpp>

#include "DebugDraw.hpp"
#include "DebugDrawComponent.hpp"

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
    : public ProjectiveRenderingSubsystem
    , public OverlayRenderingSubsystem
    , public CollectionSubsystem<DebugDrawComponent>
    , public dd::RenderInterface
{
    Game *mGame = nullptr;
    dd::ContextHandle mContext = nullptr;

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

    void createPointLinePipeline();
    void createTextPipeline();

public:
    explicit DebugDrawSubsystem(Game *game);
    ~DebugDrawSubsystem();

    void createPipelines(RenderPassCreateInfo &render_pass_info);

    void update(const TimeDuration &dt) override;
    void render(
        const TimeDuration &dt,
        std::shared_ptr<Framebuffer> framebuffer,
        const CommandListSink &cmd_out) const override;

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
