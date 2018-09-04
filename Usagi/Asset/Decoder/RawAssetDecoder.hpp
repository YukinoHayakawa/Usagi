#pragma once

#include <istream>

namespace usagi
{
/**
 * \brief Passes istream directly to converter.
 */
struct RawAssetDecoder
{
    std::istream & operator()(std::istream &in) const
    {
        return in;
    }
};
}
