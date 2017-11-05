#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

class Sampler : Noncopyable
{
public:
    virtual ~Sampler() = default;
};

}
