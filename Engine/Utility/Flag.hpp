#pragma once

namespace yuki::utility
{
template <typename Flag>
bool matchAllFlags(const Flag &flags, const Flag &checks)
{
    Flag neg = ~flags;
    neg &= checks;
    return !neg;
}
}
