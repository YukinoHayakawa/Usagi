#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuImageView : Noncopyable
{
public:
    virtual ~GpuImageView() = default;
};
}
