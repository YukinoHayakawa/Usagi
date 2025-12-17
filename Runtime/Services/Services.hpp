#pragma once

#include <Usagi/Library/Meta/Reflection/Types/TypeLists.hpp>

#include "SimpleServiceProvider.hpp"

namespace usagi::runtime
{
/*
 * `ServiceAccess` aims to provide `System`s a defined type for accessing
 * services so `System` code doesn't have to be all in the header.
 * The instances of this class is to be injected into `System`s by the
 * executive.
 *
 * todo: for now, `ServiceAccess` simply uses `SimpleServiceProvider`. But this
 *   should be able to be customized. the problem is the `ServiceProvider` is a
 *   concept rather than an interface. maybe some C++26 techniques can solve
 *   this?
 * todo: we also need some way for adding services via this interface.
 * todo: `ServiceAccess` should only provide access to Services declared in
 *   `Ts...`.
 * todo: Executive should collect all Services required by Systems and validate
 *   that the application has actually created them before running the task
 *   graph.
 * todo: use reflection & codegen to manually build vtables for `ServiceAccess`
 *   so that it can work independent of `SimpleServiceProvider`.
 */
template <typename... Ts>
struct ServiceAccess
{
    ServiceAccess(SimpleServiceProvider & services) : services_(services) {}

    template <typename T>
#ifndef __RESHARPER__
        requires(meta::reflection::is_type_in_list<T, Ts...>())
#endif
    auto & require(
        const optional_service_name_t & service_name = default_service_name
    ) const
    {
        return services_.ensure_service<T>(service_name);
    }

    template <typename T>
#ifndef __RESHARPER__
        requires(meta::reflection::is_type_in_list<T, Ts...>())
#endif
    auto optional(
        const optional_service_name_t & service_name = default_service_name
    ) const
    {
        return services_.try_get_service<T>(service_name);
    }

private:
    SimpleServiceProvider & services_;
};

template <typename... ServiceTypes>
using ServiceProviderT =
    [:std::meta::substitute(
          ^^ServiceAccess, unique_type_list_sorted_by_names<ServiceTypes...>()
      ):];
} // namespace usagi::runtime
