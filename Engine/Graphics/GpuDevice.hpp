#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuDevice : Noncopyable
{
public:
    virtual ~GpuDevice() = default;
};
}
