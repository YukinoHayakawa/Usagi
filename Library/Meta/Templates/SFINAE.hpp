#pragma once

namespace usagi::meta
{
template <typename>
struct sfinae_probe_type
{
};

template <auto>
struct sfinae_probe_value
{
};
} // namespace usagi::meta
