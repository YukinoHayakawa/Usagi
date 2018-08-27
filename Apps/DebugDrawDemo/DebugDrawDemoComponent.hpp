#pragma once

#include <Usagi/Extension/DebugDraw/DebugDrawComponent.hpp>

namespace usagi
{
struct DebugDrawDemoComponent : DebugDrawComponent
{
    bool show_grid = true;
    bool show_labels = true;

    void draw(dd::ContextHandle ctx) override;

    void drawGrid(dd::ContextHandle ctx);
    void drawLabel(dd::ContextHandle ctx, ddVec3_In pos, const char *name);
    void drawMiscObjects(dd::ContextHandle ctx);
    void drawFrustum(dd::ContextHandle ctx);
    void drawText(dd::ContextHandle ctx);
};
}
