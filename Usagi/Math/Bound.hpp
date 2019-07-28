#pragma once

// Requires _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING
// Requires _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

namespace usagi
{
// Bounding Box

using AlignedBox2i = Eigen::AlignedBox2i;
using AlignedBox2f = Eigen::AlignedBox2f;
using AlignedBox3f = Eigen::AlignedBox3f;
}
