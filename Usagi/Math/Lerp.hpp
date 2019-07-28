#pragma once

namespace usagi
{
template <typename T, typename V>
V lerp(T t, V v0, V v1)
{
    return static_cast<V>((1 - t) * v0 + t * v1);
}

template <typename V, typename R = float>
R lerpCoefficient(V v, V v0, V v1)
{
    return static_cast<R>(static_cast<R>(v) - v0) / (static_cast<R>(v1) - v0);
}
}
