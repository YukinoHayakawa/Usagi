#pragma once

#include <vector>

#include <Usagi/Core/Math.hpp>
#include <Usagi/Graphics/ProjectiveRenderingSubsystem.hpp>
#include <Usagi/Runtime/Graphics/GpuCommandPool.hpp>
#include <Usagi/Runtime/Graphics/GraphicsPipeline.hpp>

namespace usagi
{
class Camera;
struct RenderPassCreateInfo;
class GpuBuffer;
class GpuSampler;
class Game;
}

namespace usagi::moeloop
{
/**
 * \brief
 * Requires elements to have TransformComponent and SpriteComponent.
 *
 * Sprites are rendered using the texture color as output (unlit material).
 * Prior to rendering, sprites are sorted using provided compare function.
 */
class SortedSpriteRenderingSubsystem : public ProjectiveRenderingSubsystem
{
public:
    typedef std::function<bool(Element *l, Element *r)> CompareFunc;

private:
    Game *mGame;
    CompareFunc mCompareFunc;
    std::vector<Element*> mElements;
    std::shared_ptr<RenderPass> mRenderPass;
    std::shared_ptr<GraphicsPipeline> mPipeline;
    std::shared_ptr<GpuCommandPool> mCommandPool;
    std::shared_ptr<GpuSampler> mSampler;

    struct SpriteIn;

    std::shared_ptr<GpuBuffer> mVertexBuffer;
    std::shared_ptr<GpuBuffer> mIndexBuffer;

    void createBuffers();

public:
    SortedSpriteRenderingSubsystem(Game *game, CompareFunc compare_func);

    void createPipeline(RenderPassCreateInfo render_pass_info);

    void update(const TimeDuration &dt) override;
    bool processable(Element *element) override;
    void updateRegistry(Element *element) override;
    void render(
        const TimeDuration &dt,
        std::shared_ptr<Framebuffer> framebuffer,
        const CommandListSink &cmd_out) const override;
};
}
