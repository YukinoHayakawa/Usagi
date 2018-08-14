#pragma once

namespace usagi
{
class GpuImageView;
}
#define ImTextureID ImTextureID
using ImTextureID = usagi::GpuImageView *;
