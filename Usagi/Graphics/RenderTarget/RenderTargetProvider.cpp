#include "RenderTargetProvider.hpp"

#include "RenderTargetSource.hpp"

std::shared_ptr<usagi::RenderTargetSource>
usagi::RenderTargetProvider::findSource(const std::string &name) const
{
    const auto i = mSources.find(name);
    if(i == mSources.end())
        throw std::runtime_error(
            "Could not find specified render target source");
    return i->second;
}

void usagi::RenderTargetProvider::resize(const Vector2u32 &size)
{
    for(auto &&s : mSources)
    {
        s.second->resize(size);
    }
    mSize = size;
}

void usagi::RenderTargetProvider::addSharedSource(
    const std::string &name,
    std::shared_ptr<RenderTargetSource> source)
{
    const auto r = mSources.insert({ name, std::move(source) });
    if(!r.second)
    {
        throw std::runtime_error("Source name conflicted");
    }
}

std::shared_ptr<usagi::RenderTargetSource>
usagi::RenderTargetProvider::sharedColor(const std::string &name)
{
    const auto i = mSources.find(name);
    if(i == mSources.end())
        return findSource("color");
    return i->second;
}

std::shared_ptr<usagi::RenderTargetSource> usagi::RenderTargetProvider::depth()
{
    return findSource("depth");
}
