#pragma once

#include <random>

#include "Sampler.hpp"

namespace usagi
{
class RandomSampler : public Sampler
{
    std::mt19937 mEngine { std::random_device { }() };
    std::uniform_real_distribution<float> mDistribution;

public:
    float next1D() override;
    Vector2f next2D() override;
    Vector3f next3D() override;
};
}
