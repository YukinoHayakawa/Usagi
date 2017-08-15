#pragma once

#include <Usagi/Engine/Runtime/Renderable.hpp>

namespace yuki
{

class DebugFullscreenTriangle : public Renderable
{
    std::shared_ptr<class GDPipeline> mPipeline;

public:
    DebugFullscreenTriangle(std::shared_ptr<GraphicsDevice> graphics_device);

    void render(const Clock &render_clock) override;
};

}
