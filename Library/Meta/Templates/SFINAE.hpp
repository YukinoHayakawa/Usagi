#pragma once

namespace usagi::meta
{
/* SFINAE helper. Helps you to fail a concept without causing compile error. */
template <typename>
struct sfinae_probe_type
{
};

/* SFINAE helper. Helps you to fail a concept without causing compile error. */
template <auto>
struct sfinae_probe_value
{
};
} // namespace usagi::meta
