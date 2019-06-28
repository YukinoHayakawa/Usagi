#pragma once

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
        default: throw std::runtime_error("Invalid shader stage");
    }
}
}
