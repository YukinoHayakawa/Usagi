#pragma once

#include <type_traits>

namespace yuki
{

// https://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number
template <typename UInt>
UInt roundUpUnsigned(UInt num_to_round, UInt multiple)
{
    static_assert(std::is_unsigned_v<UInt>, "UInt is not unsigned type.");

    if(multiple == 0)
        return num_to_round;

    const UInt remainder = num_to_round % multiple;
    if(remainder == 0)
        return num_to_round;

    return num_to_round + multiple - remainder;
}

}
