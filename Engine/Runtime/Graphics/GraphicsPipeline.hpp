#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class RenderPass;

class GraphicsPipeline : Noncopyable
{
public:
    virtual ~GraphicsPipeline() = default;
};
}
