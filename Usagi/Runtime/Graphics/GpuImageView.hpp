#pragma once

#include <memory>

#include "ShaderResource.hpp"

namespace usagi
{
class GpuImageView
    : public ShaderResource
    // allow external graphics system to store the texture handle
    , public std::enable_shared_from_this<GpuImageView>
{
public:
    virtual ~GpuImageView() = default;
};
}
