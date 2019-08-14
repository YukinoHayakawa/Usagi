#pragma once

#include <Usagi/Core/Exception.hpp>

namespace usagi
{
enum class ShaderStage
{
    VERTEX,
    FRAGMENT,
};

inline const char * to_string(const ShaderStage stage)
{
    switch(stage)
    {
        case ShaderStage::VERTEX: return "Vertex";
        case ShaderStage::FRAGMENT: return "Fragment";
        default: USAGI_THROW(std::runtime_error("Invalid shader stage"));
    }
}
}
