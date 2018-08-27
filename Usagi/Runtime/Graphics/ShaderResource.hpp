#pragma once

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
class ShaderResource : Noncopyable
{
public:
    virtual ~ShaderResource() = default;
};
}
