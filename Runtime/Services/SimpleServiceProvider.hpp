#pragma once

#include <any>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>

#include <Usagi/Library/Memory/Noncopyable.hpp>
#include <Usagi/Runtime/RAII/RawHandleResource.hpp>

#include "Exceptions.hpp"
#include "ServiceProvider.hpp"

namespace usagi::runtime
{
/**
 * \brief A simple implementation of the `ServiceProvider` concept.
 *
 * Shio: This class uses a `std::unordered_map` with `std::any` to store
 * service instances. This implementation takes ownership of the services
 * passed to it by storing them as `std::shared_ptr` inside `std::any`.
 * Because `std::any` requires the payload to be copy-constructible.
 */
class SimpleServiceProvider
    : public RawHandleResource<std::shared_mutex *>
    , Noncopyable
{
protected:
    std::unordered_map<std::string, std::any> mServices;

    using AnyAndSharedLock =
        std::pair<std::any *, std::shared_lock<std::shared_mutex>>;
    AnyAndSharedLock try_get_service_impl_locked(std::string_view service_name);
    bool create_service_impl(std::string_view service_name, std::any instance);

    template <typename ServiceT>
    static std::string_view
        get_actual_service_name(const optional_service_name_t & service_name)
    {
        return service_name.index() == 0 ? generate_service_name<ServiceT>()
                                         : std::get<1>(service_name);
    }

public:
    static constexpr bool is_thread_safe_v = true;

    static constexpr bool is_thread_safe() { return is_thread_safe_v; }

    static constexpr bool is_polymorphic_v = true;

    static constexpr bool is_polymorphic() { return is_polymorphic_v; }

    SimpleServiceProvider();
    virtual ~SimpleServiceProvider() = default;

    SimpleServiceProvider(SimpleServiceProvider && other) noexcept = default;
    SimpleServiceProvider &
        operator=(SimpleServiceProvider && other) noexcept = default;

    template <typename ServiceT>
    static std::string_view generate_service_name()
    {
        return typeid(ServiceT).name();
    }

    template <typename ServiceT>
    auto try_get_service(
        const optional_service_name_t & service_name = default_service_name
    ) -> maybe_service_t<ServiceT>
    {
        const auto actual_service_name =
            get_actual_service_name<ServiceT>(service_name);

        auto [service_any, lock] =
            try_get_service_impl_locked(actual_service_name);

        if(!service_any)
        {
            return std::unexpected(ServiceProviderErrorCodes::ServiceNotFound);
        }

        // Shio: The `any` stores a `std::shared_ptr` to a service instance.
        // `std::any_cast` is used to retrieve it. So long as the requested
        // `ServiceT` is a type that the stored `std::shared_ptr` can be
        // converted to, this operation will succeed.
        if(auto * service_ptr_ptr =
               std::any_cast<std::shared_ptr<ServiceT>>(service_any))
        {
            auto * raw_ptr = service_ptr_ptr->get();
            // Shio: The raw pointer to the service is stable even if the map
            // rehashes. We can release the lock now.
            lock.unlock();
            return std::ref(*raw_ptr);
        }

        // Shio: If we are here, it means the service name exists, but the
        // type is not what we are looking for.
        return std::unexpected(
            ServiceProviderErrorCodes::MismatchedServiceType
        );
    }

    template <typename ServiceT>
    ServiceT & ensure_service(
        const optional_service_name_t & service_name = default_service_name
    )
    {
        const auto actual_service_name =
            get_actual_service_name<ServiceT>(service_name);
        auto result = try_get_service<ServiceT>(actual_service_name);
        if(!result)
        {
            // Shio: Consider logging the error code here for debugging.
            throw MissingRequiredRuntimeService(
                std::string(actual_service_name)
            );
        }
        return result.value().get();
    }

    template <typename ServiceT>
    auto create_default_service(
        const optional_service_name_t & service_name = default_service_name
    ) -> maybe_service_t<ServiceT>
    {
        return create_service<ServiceT>(
            service_name, std::make_unique<ServiceT>()
        );
    }

    template <typename ServiceT>
    auto create_service(
        std::unique_ptr<ServiceT>       instance,
        const optional_service_name_t & service_name = default_service_name
    ) -> maybe_service_t<ServiceT>
    {
        if(!instance)
        {
            return std::unexpected(
                ServiceProviderErrorCodes::NullServiceInstance
            );
        }

        auto * const service_ptr = instance.get();
        const auto   actual_service_name =
            get_actual_service_name<ServiceT>(service_name);

        if(!create_service_impl(
               actual_service_name,
               std::shared_ptr<ServiceT>(std::move(instance))
           ))
        {
            return std::unexpected(ServiceProviderErrorCodes::ServiceNameInUse);
        }

        return std::ref(*service_ptr);
    }

    template <typename ServiceT, typename... Args>
    auto create_service(
        std::in_place_t,
        const optional_service_name_t & service_name,
        auto &&                         args
    ) -> maybe_service_t<ServiceT>
    {
        return create_service<ServiceT>(
            service_name,
            std::make_unique<ServiceT>(std::forward<Args>(args)...)
        );
    }

    template <typename ServiceT, typename... Args>
    auto create_service(std::in_place_t, auto && args)
        -> maybe_service_t<ServiceT>
    {
        return create_service<ServiceT>(
            std::in_place, default_service_name, std::forward<Args>(args)...
        );
    }
};

static_assert(ServiceProvider<SimpleServiceProvider>);
static_assert(NamelessServiceProvider<SimpleServiceProvider>);
} // namespace usagi::runtime
