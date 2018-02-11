#pragma once

namespace yuki::utility
{
template <typename Flag>
bool matchAllFlags(const Flag &flags, const Flag &checks)
{
    return (!flags & checks) == 0;
}
}
