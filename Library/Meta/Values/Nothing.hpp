#pragma once

namespace usagi
{
// Helper class for `optional`, etc.
struct Nothing
{
    constexpr Nothing() = default;

    constexpr bool operator==(const Nothing &) const { return true; }
};
} // namespace usagi
