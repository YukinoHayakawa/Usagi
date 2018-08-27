#pragma once

#include <eigen3/Eigen/Core>

namespace yuki
{
struct Intersection
{
    Eigen::Vector3f position;
    float distance;
    Eigen::Vector3f normal;
};
}
