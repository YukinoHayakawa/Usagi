#pragma once

namespace usagi
{
constexpr bool platform_dependent = true;
} // namespace usagi

#ifdef __RESHARPER__
    #define USAGI_PLATFORM_DEPENDENT
#else
    #define USAGI_PLATFORM_DEPENDENT [[= usagi::platform_dependent]]
#endif
