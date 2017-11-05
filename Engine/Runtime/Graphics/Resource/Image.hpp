#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

class Image : Noncopyable
{
public:
    virtual ~Image() = default;
};

}
