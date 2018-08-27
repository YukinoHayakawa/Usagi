#pragma once

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
class GpuSemaphore : Noncopyable
{
public:
    virtual ~GpuSemaphore() = default;
};
}
