#pragma once

#define DEBUG_DRAW_EXPLICIT_CONTEXT
#include <debug-draw/debug_draw.hpp>

namespace dd
{
void xySquareGrid(
    DD_EXPLICIT_CONTEXT_ONLY(ContextHandle ctx,)
    float mins,
    float maxs,
    float z,
    float step,
    ddVec3_In color,
    int durationMillis = 0,
    bool depthEnabled = true);
}
