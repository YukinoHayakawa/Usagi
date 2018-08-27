#pragma once

namespace usagi
{
enum class GpuImageComponentSwizzle
{
    IDENTITY,
    ZERO,
    ONE,
    R,
    G,
    B,
    A,
};

struct GpuImageComponents
{
    GpuImageComponentSwizzle r = GpuImageComponentSwizzle::IDENTITY;
    GpuImageComponentSwizzle g = GpuImageComponentSwizzle::IDENTITY;
    GpuImageComponentSwizzle b = GpuImageComponentSwizzle::IDENTITY;
    GpuImageComponentSwizzle a = GpuImageComponentSwizzle::IDENTITY;
};

struct GpuImageViewCreateInfo
{
    GpuImageComponents components;
};
}
