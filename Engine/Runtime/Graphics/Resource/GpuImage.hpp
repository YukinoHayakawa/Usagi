#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
struct GpuImageViewCreateInfo;
class GpuImageView;

class GpuImage : Noncopyable
{
public:
    virtual ~GpuImage() = default;

    virtual std::shared_ptr<GpuImageView> baseView() = 0;
    virtual std::shared_ptr<GpuImageView> createView(
        const GpuImageViewCreateInfo &info) = 0;

    virtual void upload(void *data, std::size_t size)
    {
        throw std::runtime_error("Operation not supported.");
    }
};
}
