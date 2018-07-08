#pragma once

#include <iterator>

namespace usagi::utility
{
/**
 * \brief Find the beginning of the first consecutive sequence of a particular
 * value. The value must repeat in the sequence for at least the spcified times.
 * \tparam ForwardIterator 
 * \tparam T 
 * \param begin 
 * \param end 
 * \param value 
 * \param min_count 
 * \return 
 */
template <typename ForwardIterator, typename T>
ForwardIterator findFirstConsecutive(ForwardIterator begin, ForwardIterator end,
    const T &value, const std::size_t min_count)
{
    auto ret = begin;
    std::size_t count = 0;
    for(; count < min_count && begin != end; ++begin)
    {
        if(*begin != value)
        {
            ret = std::next(begin);
            count = 0;
        }
        else
        {
            ++count;
        }
    }
    if(count < min_count) ret = end;
    return ret;
}
}
