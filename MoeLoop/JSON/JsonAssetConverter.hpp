#pragma once

#include <memory>

#include "JSON.hpp"
#include "JsonAssetDecoder.hpp"

namespace usagi::moeloop
{
template <typename T>
struct JsonAssetConverter
{
    using DefaultDecoder = JsonAssetDecoder;

    template <typename... Args>
    std::shared_ptr<T> operator()(const json &j, Args &&...args) const
    {
        auto r = std::make_shared<T>(std::forward<Args>(args)...);
        *r = j;
        return std::move(r);
    }
};
}
