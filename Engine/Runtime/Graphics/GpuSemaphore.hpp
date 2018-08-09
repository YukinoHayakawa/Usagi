#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuSemaphore : Noncopyable
{
public:
    virtual ~GpuSemaphore() = default;
};
}
