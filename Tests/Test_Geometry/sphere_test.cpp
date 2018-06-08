#include "pch.h"

#include <Usagi/Engine/Geometry/Shape/Sphere.hpp>

using namespace yuki;

TEST(SphereRayIntersectionTest, AlmostIntersect)
{
    Sphere s;
    EXPECT_TRUE(s.intersect({ { 1, 1, 1 }, { }}))
    
}
