#pragma once

#include <cstdint>

namespace yuki
{

class GraphicsSemaphore
{
public:
    virtual ~GraphicsSemaphore() = default;

    virtual uint64_t getSemaphoreHandle() const = 0;
    virtual uint64_t getNativePipelineStage() const = 0;
};

}
