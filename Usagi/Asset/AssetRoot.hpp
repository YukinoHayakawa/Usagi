#pragma once

#include <boost/uuid/uuid.hpp>

#include <Usagi/Core/Element.hpp>

#include "Asset.hpp"

namespace usagi
{
class AssetPackage;
class AssetPath;

class AssetRoot : public Element
{
    Asset * findAsset(std::string locator) const;
    Asset * findAssetByUuid(const boost::uuids::uuid &uuid) const;
    Asset * findAssetByString(std::string string) const;

    bool acceptChild(Element *child) override;

    template <
        typename ConvertorT,
        typename DecoderT = typename ConvertorT::DefaultDecoder,
        typename... Args
    >
    struct FindHelper
    {
        using ReturnT = decltype(std::declval<ConvertorT>()(
            std::declval<decltype(std::declval<DecoderT>()(
                std::declval<std::istream&>()))&>(),
            std::declval<Args>()...
        ));
        using SubresourceT = typename ReturnT::element_type;
    };

public:
    explicit AssetRoot(Element *parent);

    template <
        typename ConvertorT,
        typename DecoderT = typename ConvertorT::DefaultDecoder,
        typename... Args
    >
    auto find(const std::string &locator, Args &&...convertor_args)
        -> typename FindHelper<ConvertorT, DecoderT, Args...>::ReturnT
    {
        const auto asset = findAsset(locator);

        // found in cache
        if(auto res = asset->subresource<
            typename FindHelper<ConvertorT, DecoderT, Args...>::SubresourceT
        >())
            return std::move(res);

        // load from stream
        const auto in = asset->open();
        auto res = ConvertorT()(
            DecoderT()(*in),
            std::forward<Args>(convertor_args)...);
        asset->addSubresource(res);
        return std::move(res);
    }
};
}
