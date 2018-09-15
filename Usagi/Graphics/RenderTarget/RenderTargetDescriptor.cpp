#include "RenderTargetDescriptor.hpp"

#include <Usagi/Runtime/Graphics/GpuDevice.hpp>

#include "RenderTargetProvider.hpp"
#include "RenderTargetSource.hpp"
#include "RenderTarget.hpp"

usagi::RenderTargetDescriptor::RenderTargetDescriptor(
    RenderTargetProvider *provider)
    : mProvider(provider)
{
}

std::unique_ptr<usagi::RenderTarget> usagi::RenderTargetDescriptor::finish()
{
    RenderPassCreateInfo render_pass_info;
    for(std::size_t i = 0; i < mSources.size(); ++i)
    {
        render_pass_info.attachment_usages.push_back(
            mSources[i]->usage(mUsages[i]));
    }
    auto render_pass = mProvider->gpu()->createRenderPass(render_pass_info);
    return std::make_unique<RenderTarget>(
        mProvider,
        std::move(render_pass),
        std::move(mSources));
}

void usagi::RenderTargetDescriptor::sharedColorTarget(
    const std::string &name,
    const GpuAttachmentUsage &usage)
{
    mSources.push_back(mProvider->sharedColor(name));
    mUsages.push_back(usage);
}

void usagi::RenderTargetDescriptor::depthTarget(const GpuAttachmentUsage &usage)
{
    mSources.push_back(mProvider->depth());
    mUsages.push_back(usage);
}
