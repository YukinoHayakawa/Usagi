#pragma once

#include <concepts>
#include <memory>
#include <type_traits>

namespace usagi
{
template <typename T>
concept Service = requires (T t)
{
    { T::ServiceT };
    { t.get_service() } -> std::convertible_to<typename T::ServiceT &>;
};

template <typename ServiceBaseT>
struct SimplePlatformService
{
    using ServiceT = ServiceBaseT;

    ServiceT & get_service()
    {
        return *mServiceImpl.get();
    }

    // A dispatch tag is used because template parameters of a constructor
    // cannot be specified explicitly.
    // See: https://stackoverflow.com/questions/26553803/derive-from-template-constructor-of-template-base-class
    template <typename ServiceImplT, typename... Args>
    explicit SimplePlatformService(Tag<ServiceImplT>, Args &&... args) requires
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

// Cast the Runtime to the specified Service Implementation.
// The client this macro should however only rely assume an interface provided
// by the according Service Interface.
// #define ServiceInterface___ ServiceImpl
#define USAGI_SERVICE(rt, svc) \
    static_cast<svc&>(rt).get_service() \
/**/

// Option actions that only performed when the specified Service is present.
#define USAGI_OPT_SERVICE(rt, svc_type, svc_var, task) \
    do { \
        if constexpr(std::is_base_of_v< \
            svc_type, std::remove_cvref_t<decltype(rt)>>) \
        { \
            auto &svc_var = USAGI_SERVICE(rt, svc_type); \
            task \
        } \
    } while(false) \
/**/

#define USAGI_DECL_PLATFORM_SERVICE(svc_name, base) \
using svc_name = ::usagi::SimplePlatformService<base>; \
/**/
