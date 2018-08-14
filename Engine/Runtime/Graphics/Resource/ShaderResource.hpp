#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class ShaderResource : Noncopyable
{
public:
    virtual ~ShaderResource() = default;
};
}
