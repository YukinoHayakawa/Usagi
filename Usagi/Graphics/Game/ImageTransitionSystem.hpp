#pragma once

#include "RenderableSystem.hpp"

namespace usagi
{
class GpuDevice;
class GpuCommandPool;

class ImageTransitionSystem : public RenderableSystem
{
    std::shared_ptr<GpuCommandPool> mCommandPool;

public:
    explicit ImageTransitionSystem(GpuDevice *gpu);

    void onElementComponentChanged(Element *element) override;
    void update(const Clock &clock) override;
    void createRenderTarget(RenderTargetDescriptor &descriptor) override;
    void createPipelines() override;
    std::shared_ptr<GraphicsCommandList> render(const Clock &clock) override;
};
}
