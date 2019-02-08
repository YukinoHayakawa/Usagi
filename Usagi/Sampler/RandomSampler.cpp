#include "RandomSampler.hpp"

float usagi::RandomSampler::next1D()
{
    return mDistribution(mEngine);
}

usagi::Vector2f usagi::RandomSampler::next2D()
{
    return { next1D(), next1D() };
}

usagi::Vector3f usagi::RandomSampler::next3D()
{
    return { next1D(), next1D(), next1D() };
}
