#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuImageView;

class GpuImage : Noncopyable
{
public:
    virtual ~GpuImage() = default;

    virtual GpuImageView * fullView() = 0;
};
}
