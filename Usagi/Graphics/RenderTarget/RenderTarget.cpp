#include "RenderTarget.hpp"

#include <Usagi/Runtime/Graphics/GpuDevice.hpp>

#include "RenderTargetProvider.hpp"
#include "RenderTargetSource.hpp"

usagi::RenderTarget::RenderTarget(
    RenderTargetProvider *provider,
    std::shared_ptr<RenderPass> render_pass,
    std::vector<std::shared_ptr<RenderTargetSource>> render_target_sources)
    : mProvider(provider)
    , mRenderPass(std::move(render_pass))
    , mSources(std::move(render_target_sources))
{
}

std::shared_ptr<usagi::Framebuffer> usagi::RenderTarget::createFramebuffer()
{
    // stored as a member to prevent frequent memory allocation
    mImageViews.clear();
    for(auto &&s : mSources)
    {
        mImageViews.push_back(s->view());
    }
    return mProvider->gpu()->createFramebuffer(mProvider->size(), mImageViews);
}
