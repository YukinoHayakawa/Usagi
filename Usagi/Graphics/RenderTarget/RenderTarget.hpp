#pragma once

#include <memory>
#include <vector>

namespace usagi
{
class Framebuffer;
class RenderPass;
class GpuImageView;
class RenderTargetSource;
class RenderTargetProvider;

class RenderTarget
{
    RenderTargetProvider *mProvider = nullptr;
    std::shared_ptr<RenderPass> mRenderPass;
    std::vector<std::shared_ptr<RenderTargetSource>> mSources;
    std::vector<std::shared_ptr<GpuImageView>> mImageViews;

public:
    RenderTarget(
        RenderTargetProvider *provider,
        std::shared_ptr<RenderPass> render_pass,
        std::vector<std::shared_ptr<RenderTargetSource>> render_target_sources);

    std::shared_ptr<RenderPass> renderPass() const { return mRenderPass; }
    std::shared_ptr<Framebuffer> createFramebuffer();
};
}
