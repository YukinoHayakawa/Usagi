#pragma once

#include <Usagi/Core/Math.hpp>

#include "JSON.hpp"

namespace Eigen
{
using namespace usagi::moeloop;

// Vector2

template <typename T>
void to_json(json& j, const Eigen::Matrix<T, 2, 1>& v)
{
    j = json { { "x", v.x() }, { "y", v.y() } };
}

template <typename T>
void from_json(const json& j, Eigen::Matrix<T, 2, 1>& v)
{
    v.x() = j.at("x").get<T>();
    v.y() = j.at("y").get<T>();
}

// Vector3

template <typename T>
void to_json(json& j, const Eigen::Matrix<T, 3, 1>& v)
{
    j = json { { "x", v.x() }, { "y", v.y() }, { "z", v.z() } };
}

template <typename T>
void from_json(const json& j, Eigen::Matrix<T, 3, 1>& v)
{
    v.x() = j.at("x").get<T>();
    v.y() = j.at("y").get<T>();
    v.z() = j.at("z").get<T>();
}
}
