#include "JsonAssetDecoder.hpp"

#include <nlohmann/json.hpp>

usagi::moeloop::json usagi::moeloop::JsonAssetDecoder::operator()(
    std::istream &in) const
{
    json j;
    in >> j;
    return std::move(j);
}
