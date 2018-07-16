#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class GraphicsPipelineCompiler;
class GpuImage;

class GpuDevice : Noncopyable
{
    /**
     * \brief Counter for reclaiming resources. Increased by one before every
     * frame. (Better than starting from 0 because resources never used will
     * be considered to belong to frame 0, which is before all frames.)
     */
    std::uint64_t mCurrentFrameIndex = 0;

public:
    virtual ~GpuDevice() = default;

    std::uint64_t currentFrameIndex() const { return mCurrentFrameIndex; }
    virtual GpuImage * swapChainImage() = 0;

    virtual std::unique_ptr<GraphicsPipelineCompiler> createPipelineCompiler()
        = 0;
};
}
