#pragma once

#include <map>

#include <Usagi/Core/Math.hpp>

namespace usagi
{
struct GpuAttachmentOps;
class RenderTargetSource;
class GpuDevice;

class RenderTargetProvider
{
protected:
    using SourceMap = std::map<std::string, std::shared_ptr<RenderTargetSource>>;
    SourceMap mSources;
    // std::vector<std::weak_ptr<RenderTargetSource>> mOtherSources;
    Vector2u32 mSize;

    std::shared_ptr<RenderTargetSource> findSource(
        const std::string &name) const;

public:
    virtual ~RenderTargetProvider() = default;

    Vector2u32 size() const { return mSize; }
    void resize(const Vector2u32 &size);
    void addSharedSource(
        const std::string &name,
        std::shared_ptr<RenderTargetSource> source);

    std::shared_ptr<RenderTargetSource> sharedColor(const std::string &name);
    std::shared_ptr<RenderTargetSource> depth();

    virtual GpuDevice * gpu() const = 0;
};
}
