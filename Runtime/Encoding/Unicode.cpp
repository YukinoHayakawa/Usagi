#include "Unicode.hpp"

namespace usagi
{
std::string_view without_utf8_bom(std::string_view view)
{
    if(view.substr(0, 3) == "\xef\xbb\xbf")
        return view.substr(3);
    return view;
}
}
