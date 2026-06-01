#pragma once

namespace usagi
{
/*
 * A dummy function for causing compile error within a constexpr function if a
 * wrong branch was taken. Useful for checking invariants.
 */
inline void raise_compile_error(const char *msg)
{
}
} // namespace usagi
