#pragma once

#include <Usagi/Graphics/Game/ProjectiveRenderingSystem.hpp>
#include <Usagi/Graphics/Game/OverlayRenderingSystem.hpp>
#include <Usagi/Game/CollectionSystem.hpp>

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

class DebugDrawSystem final
    : public ProjectiveRenderingSystem
    , public OverlayRenderingSystem
    , public CollectionSystem<DebugDrawComponent>
    , public dd::RenderInterface
{
    Game *mGame = nullptr;
    dd::ContextHandle mContext = nullptr;

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
    explicit DebugDrawSystem(Game *game);
    ~DebugDrawSystem();

    void createRenderTarget(RenderTargetDescriptor &descriptor) override;
    void createPipelines() override;
    void update(const Clock &clock) override;
    std::shared_ptr<GraphicsCommandList> render(const Clock &clock) override;

    const std::type_info & type() override
    {
        return typeid(decltype(*this));
    }
    
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
