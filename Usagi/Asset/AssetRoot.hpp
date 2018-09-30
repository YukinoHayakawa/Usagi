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
            std::declval<AssetLoadingContext*>(),
            std::declval<decltype(std::declval<DecoderT>()(
                std::declval<std::istream&>()))&>(),
            std::declval<Args>()...
        ));
    };

    /**
     * \brief Note that uncached loading may return any value, but cached
     * loading may only return a shared_ptr in order to store in the cache.
     * \tparam ConverterT
     * \tparam DecoderT
     * \tparam AllowCache
     * \tparam Args
     * \param locator
     * \param converter_args
     * \return
     */
    template <
        typename ConverterT,
        typename DecoderT,
        bool AllowCache,
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
        if constexpr(AllowCache)
        {
            if(auto res = ctx.asset->subresource<typename FindHelper<
                ConverterT, DecoderT, Args...>::ReturnT::element_type
            >()) return std::move(res);
        }

        // load from stream
        // not using Asset::decode() so the lifetime of opened istream is in
        // our control
        const auto in = ctx.asset->open();
        auto res = ConverterT()(
            // converter can use the context to request additional resources
            &ctx,
            DecoderT()(*in),
            std::forward<Args>(converter_args)...);
        if constexpr(AllowCache) ctx.asset->addSubresource(res);
        return std::move(res);
    }

public:
    AssetRoot(Element *parent, std::string name);

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
