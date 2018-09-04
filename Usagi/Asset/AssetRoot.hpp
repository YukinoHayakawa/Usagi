#pragma once

#include <boost/uuid/uuid.hpp>

#include <Usagi/Core/Element.hpp>

#include "Asset.hpp"
#include "AssetLoadingContext.hpp"

namespace usagi
{
class AssetPackage;
class AssetPath;

class AssetRoot : public Element
{
    Asset * findAssetByUuid(const boost::uuids::uuid &uuid) const;
    Asset * findAssetByString(std::string string) const;

    bool acceptChild(Element *child) override;

    template <
        typename ConverterT,
        typename DecoderT = typename ConverterT::DefaultDecoder,
        typename... Args
    >
    struct FindHelper
    {
        using ReturnT = decltype(std::declval<ConverterT>()(
            std::declval<AssetRoot*>(),
            std::declval<decltype(std::declval<DecoderT>()(
                std::declval<std::istream&>()))&>(),
            std::declval<Args>()...
        ));
        using SubresourceT = typename ReturnT::element_type;
    };

    template <
        typename ConverterT,
        typename DecoderT,
        bool Cached,
        typename... Args
    >
    auto locateSubresource(
        const std::string &locator,
        Args &&...converter_args)
        -> typename FindHelper<ConverterT, DecoderT, Args...>::ReturnT
    {
        AssetLoadingContext ctx;
        ctx.asset_root = this;
        ctx.locator = locator;
        ctx.asset = findAsset(locator);

        // found in cache
        if constexpr(Cached)
        {
            if(auto res = ctx.asset->subresource<
                typename FindHelper<ConverterT, DecoderT, Args...>::SubresourceT
            >()) return std::move(res);
        }

        // load from stream
        auto res = ConverterT()(
            // converter can use the context to request additional resources
            &ctx,
            ctx.asset->decode<DecoderT>(),
            std::forward<Args>(converter_args)...);
        if constexpr(Cached) ctx.asset->addSubresource(res);
        return std::move(res);
    }

public:
    explicit AssetRoot(Element *parent);

    Asset * findAsset(std::string locator) const;

    template <
        typename ConverterT,
        typename DecoderT = typename ConverterT::DefaultDecoder,
        typename... Args
    >
    auto res(const std::string &locator, Args &&...converter_args)
    {
        return locateSubresource<ConverterT, DecoderT, true>(
            locator,
            std::forward<Args>(converter_args)...
        );
    }

    template <
        typename ConverterT,
        typename DecoderT = typename ConverterT::DefaultDecoder,
        typename... Args
    >
    auto uncachedRes(const std::string &locator, Args &&...converter_args)
    {
        return locateSubresource<ConverterT, DecoderT, false>(
            locator,
            std::forward<Args>(converter_args)...
        );
    }
};
}
