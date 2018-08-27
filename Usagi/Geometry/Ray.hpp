#pragma once

#include <eigen3/Eigen/Core>

namespace yuki
{
struct Ray
{
    Eigen::Vector3f origin;
    Eigen::Vector3f direction;

    Ray(Eigen::Vector3f origin, Eigen::Vector3f direction)
        : origin { std::move(origin) }
        , direction { std::move(direction) }
    {
    }
};
}
