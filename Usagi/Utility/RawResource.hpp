#pragma once

#include "TypeTrait.hpp"

namespace usagi
{
/**
 * \brief A wrapper class for auto releasing manually managed resources
 * \tparam ResT
 * \tparam DeleterT
 */
template <typename ResT, typename DeleterT>
class RawResource
{
    ResT mResource;
    DeleterT mDeleter;

public:
    template <typename ConstructorT>
    RawResource(ResT res, ConstructorT constructor, DeleterT deleter)
        : mResource(std::move(res))
        , mDeleter(std::move(deleter))
    {
        constructor(mResource);
    }

    ~RawResource()
    {
        mDeleter(mResource);
    }

    RawResource(const RawResource &other) = delete;
    RawResource(RawResource &&other) noexcept = delete;
    RawResource & operator=(const RawResource &other) = delete;
    RawResource & operator=(RawResource &&other) noexcept = delete;

    // implicit convertor to raw type
    operator ResT() const
    {
        return mResource;
    }

    ResT val()
    {
        return mResource;
    }

    ResT & ref()
    {
        return mResource;
    }

    ResT * addr()
    {
        return &mResource;
    }
};
}

#define USAGI_TRIVIAL_RAW_RESOURCE(type, name, ctor, dtor) \
::usagi::RawResource name { \
    DefaultConstructedValue<type>, \
    [&](type &name) ctor, \
    [&](type &name) dtor \
} \
/**/
