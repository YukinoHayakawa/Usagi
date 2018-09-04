#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>

#include <Usagi/Core/Element.hpp>
#include <Usagi/Utility/TypeCast.hpp>

namespace usagi
{
class Asset : public Element
{
protected:
    const boost::uuids::uuid mUuid;

    /**
     * \brief Different types of resources may be derived from a single asset.
     * For example, a JSON may be used to load a scene or a character.
     * This map stores weak references to the asset as a cache. An external
     * strong reference must be held to retain the loaded status of the
     * subresource.
     */
    std::map<std::type_index, std::any> mSubresources;

public:
    Asset(
        Element *parent,
        std::string name,
        boost::uuids::uuid uuid = boost::uuids::nil_uuid()
	);

    virtual std::unique_ptr<std::istream> open() = 0;

    boost::uuids::uuid uuid() const { return mUuid; }

    /**
     * \brief The human-friendly string for locating the asset, including
     * the package name.
     * \return
     */
    virtual std::string path() const = 0;

    /**
     * \brief A path that can be used to locate assets in the same directory
     * by appending the target asset name. i.e. trailing space is included.
     * \return
     */
    virtual std::string parentPath() const = 0;

    template <typename DecoderT>
    auto decode()
    {
        return DecoderT()(*open());
    }

    template <typename SubresourceT>
    std::shared_ptr<SubresourceT> subresource() const
    {
        const auto i = mSubresources.find(typeid(SubresourceT));
        if(i == mSubresources.end()) return { };
        return std::any_cast<std::weak_ptr<SubresourceT>>(i->second).lock();
    }

    template <typename SubresourceT>
    void addSubresource(std::shared_ptr<SubresourceT> &res)
    {
        const auto i = mSubresources.insert({
            typeid(SubresourceT), std::weak_ptr<SubresourceT>(res)
        });
        assert(i.second);
    }
};
}
