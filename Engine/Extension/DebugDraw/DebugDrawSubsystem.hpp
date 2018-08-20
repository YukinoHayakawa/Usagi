#pragma once

#include <set>

#include <Usagi/Engine/Runtime/Graphics/RenderableSubsystem.hpp>

#include "DebugDraw.hpp"

namespace usagi
{
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
    std::shared_ptr<GpuCommandPool> mCommandPool;
    std::shared_ptr<GraphicsCommandList> mCurrentCmdList;
    std::shared_ptr<GpuBuffer> mVertexBuffer;

    void createPointLinePipeline(RenderPassCreateInfo &render_pass_info);

public:
    explicit DebugDrawSubsystem(Game *game);
    ~DebugDrawSubsystem();

    void createPipelines(RenderPassCreateInfo &render_pass_info);

    void update(
        const TimeDuration &dt,
        std::shared_ptr<Framebuffer> framebuffer,
        const CommandListSink &cmd_out) override;
    bool processable(Element *element) override;
    void updateRegistry(Element *element) override;

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
