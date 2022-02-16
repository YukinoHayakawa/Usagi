#pragma once

namespace usagi
{
template <typename T>
bool match_bit_mask(T candidate_mask, T required_bits)
{
    return (candidate_mask & required_bits) == required_bits;
}
}
