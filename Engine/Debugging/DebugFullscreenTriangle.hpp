#pragma once

#include <memory>

#include <Usagi/Engine/Runtime/Renderable.hpp>

namespace yuki
{

class DebugFullscreenTriangle : public Renderable
{
    std::shared_ptr<class GDPipeline> mPipeline;

public:
    DebugFullscreenTriangle(GraphicsDevice &gd);
    void render(GraphicsDevice &gd, const Clock &render_clock) override;
};

}
