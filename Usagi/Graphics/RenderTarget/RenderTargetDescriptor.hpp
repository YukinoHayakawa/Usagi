#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>

namespace usagi
{
class RenderTarget;
class RenderTargetSource;
class RenderTargetProvider;

class RenderTargetDescriptor : Noncopyable
{
    RenderTargetProvider *mProvider = nullptr;
    std::vector<std::shared_ptr<RenderTargetSource>> mSources;
    std::vector<GpuAttachmentUsage> mUsages;

public:
    explicit RenderTargetDescriptor(RenderTargetProvider *provider);

    // binding points are implicitly specified by invocation order of
    // following target functions

    /**
     * \brief If no target with the same name exist, the default one will
     * be used.
     * \param name
     * \param usage
     */
    void sharedColorTarget(
        const std::string &name,
        const GpuAttachmentUsage &usage = { }
    );

    // virtual void colorTarget( format size ...) = 0;
    void depthTarget(const GpuAttachmentUsage &usage = { });

    std::unique_ptr<RenderTarget> finish();
};
}
