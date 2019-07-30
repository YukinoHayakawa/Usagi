#pragma once

namespace usagi
{
inline bool & toggle(bool &flag)
{
    return flag = !flag;
}
}
