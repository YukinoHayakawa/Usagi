#pragma once

#include <memory>
#include <type_traits>

#include <Usagi/Library/Meta/TypeContainers/TypeTag.hpp>
#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
template <typename ServiceBaseT>
struct SimplePlatformService : Noncopyable
{
    using ServiceT = ServiceBaseT;

    ServiceT &get_service()
    {
        return *mServiceImpl;
    }

    // A dispatch tag is used here because template parameters of a constructor
    // cannot be specified explicitly.
    // See: https://stackoverflow.com/questions/26553803/derive-from-template-constructor-of-template-base-class
    template <typename ServiceImplT, typename... Args>
    explicit SimplePlatformService(TypeTag<ServiceImplT>, Args &&... args) requires
        std::is_base_of_v<ServiceBaseT, ServiceImplT>
        : mServiceImpl(std::make_unique<ServiceImplT>(
            std::forward<Args>(args)...
        ))
    {
    }

    template <typename ServiceImplT, typename... Args>
    void reset(Args &&... args) requires
        std::is_base_of_v<ServiceBaseT, ServiceImplT>
    {
        mServiceImpl = std::make_unique<ServiceImplT>(
            std::forward<Args>(args)...
        );
    }

private:
    std::unique_ptr<ServiceBaseT> mServiceImpl;
};
}

#define USAGI_DECL_PLATFORM_SERVICE(svc_name, base) \
using svc_name = ::usagi::SimplePlatformService<base>; \
/**/
