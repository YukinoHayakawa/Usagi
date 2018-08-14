#pragma once

#include "ShaderResource.hpp"

namespace usagi
{
class GpuSampler : public ShaderResource
{
public:
    virtual ~GpuSampler() = default;
};
}
