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

    virtual std::shared_ptr<GpuImageView> fullView() = 0;
};
}
