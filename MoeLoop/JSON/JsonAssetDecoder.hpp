#pragma once

#include "JSONForwardDecl.hpp"

namespace usagi::moeloop
{
struct JsonAssetDecoder
{
    json operator()(std::istream &in) const;
};
}
