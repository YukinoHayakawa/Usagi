#pragma once

#include <eigen3/Eigen/Core>

namespace usagi
{
using Vector2f = Eigen::Vector2f;
using Vector2i = Eigen::Vector2i;
using Vector2u32 = Eigen::Matrix<std::uint32_t, 2, 1>;
using Vector2i32 = Eigen::Matrix<std::int32_t, 2, 1>;
using Vector3f = Eigen::Vector3f;
using Vector4f = Eigen::Vector4f;
using Color4f = Vector4f;
}
