#pragma once

#include <string>
#include <vector>

namespace yuki
{
namespace string
{

inline std::vector<wchar_t> toWideVector(const std::string &multibyte_string)
{
    std::vector<wchar_t> wide_vec(multibyte_string.size() + 1, '\0');
    std::mbstowcs(&wide_vec[0], multibyte_string.c_str(), multibyte_string.size());
    return wide_vec;
}

inline std::wstring toWideString(const std::string &multibyte_string)
{
    auto wide_vec = toWideVector(multibyte_string);
    std::wstring wide_str(&wide_vec[0]);
    return wide_str;
}

}
}
