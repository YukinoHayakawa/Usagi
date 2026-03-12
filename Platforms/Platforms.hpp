#pragma once

namespace usagi
{
constexpr bool platform_dependent = true;
} // namespace usagi

// clang-format off
#ifdef __RESHARPER__
    #define USAGI_PLATFORM_DEPENDENT
#else
    #define USAGI_PLATFORM_DEPENDENT [[=usagi::platform_dependent]]
#endif
// clang-format on
