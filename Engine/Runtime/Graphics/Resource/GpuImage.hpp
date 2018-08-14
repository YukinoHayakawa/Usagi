#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuImageView;

class GpuImage : Noncopyable
{
public:
    virtual ~GpuImage() = default;

    virtual std::shared_ptr<GpuImageView> baseView() = 0;

    virtual void upload(void *data, std::size_t size)
    {
        throw std::runtime_error("Operation not supported.");
    }
};
}
