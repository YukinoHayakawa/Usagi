#pragma once

#include <memory>

#include <Usagi/Engine/Runtime/Renderable.hpp>

namespace yuki
{

class DebugTriangle : public Renderable
{
    std::shared_ptr<class GDPipeline> mPipeline;

public:
    DebugTriangle(GraphicsDevice &gd);
    void render(GraphicsDevice &gd, const Clock &render_clock) override;
};

}
