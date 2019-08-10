#include "StringAssetConverter.hpp"

#include <Usagi/Utility/Stream.hpp>

std::string usagi::StringAssetConverter::operator()(
    AssetLoadingContext *ctx,
    std::istream &in) const
{
    return readStreamToString(in);
}
