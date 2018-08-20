#pragma once

// Requires _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING
// Requires _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

namespace usagi
{
using Vector2f = Eigen::Vector2f;
using Vector2i = Eigen::Vector2i;
using Vector2u32 = Eigen::Matrix<std::uint32_t, 2, 1>;
using Vector2i32 = Eigen::Matrix<std::int32_t, 2, 1>;
using Vector3f = Eigen::Vector3f;
using Vector4f = Eigen::Vector4f;

using Color4f = Vector4f;

using Matrix4f = Eigen::Matrix4f;

using Projective3f = Eigen::Projective3f;
}
