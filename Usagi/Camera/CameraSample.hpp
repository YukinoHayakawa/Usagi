#pragma once

#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
struct CameraSample
{
    // sample point uniformly distributed in framebuffer surface.
    // i.e. [0, framebuffer width/height]
    Vector2f screen;
};
}
