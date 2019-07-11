#define DEBUG_DRAW_IMPLEMENTATION
#include "DebugDrawSystem.hpp"

void dd::xySquareGrid(
    DD_EXPLICIT_CONTEXT_ONLY(ContextHandle ctx,)
    const float mins,
    const float maxs,
    const float z,
    const float step,
    ddVec3_In color,
    const int durationMillis,
    const bool depthEnabled)
{
    if(!isInitialized(DD_EXPLICIT_CONTEXT_ONLY(ctx))) { return; }

    ddVec3 from, to;
    for(float i = mins; i <= maxs; i += step)
    {
        // Horizontal line (along the X)
        vecSet(from, mins, i, z);
        vecSet(to, maxs, i, z);
        line(DD_EXPLICIT_CONTEXT_ONLY(ctx,) from, to, color, durationMillis,
            depthEnabled);

        // Vertical line (along the Y)
        vecSet(from, i, mins, z);
        vecSet(to, i, maxs, z);
        line(DD_EXPLICIT_CONTEXT_ONLY(ctx,) from, to, color, durationMillis,
            depthEnabled);
    }
}
