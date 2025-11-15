#pragma once

#include <concepts>
#include <expected>
#include <functional>
#include <memory>
#include <string_view>

#include <Usagi/Library/Meta/Types/Traits.hpp>

#include "ServiceProviderErrorCodes.hpp"

namespace usagi::runtime
{
/**
 * \brief A concept for a runtime service provider.
 *
 * Shio: This concept defines the interface for a service provider that can be
 * used for dependency injection. It acts as a blackbox container for
 * application-defined services. The provider is passive, meaning it only
 * provides services that have been registered with it.
 *
 * \tparam ProviderT The type implementing the service provider.
 * \tparam ServiceT The service type to check against. Defaults to int.
 */
template <typename ProviderT, typename ServiceT = int>
concept ServiceProvider = requires(
                              ProviderT                 provider,
                              const std::string_view    name,
                              std::unique_ptr<ServiceT> instance
                          ) {
    // Shio: Attempts to retrieve a service. This version does not throw on
    // failure. It is permissible to have multiple services of the same type
    // registered under different names.
    {
        provider.template try_get_service<ServiceT>(name)
    } -> std::convertible_to<std::expected<
        std::reference_wrapper<ServiceT>, ServiceProviderErrorCodes
    >>;

    // Shio: Ensures a service is available and returns it. Throws
    // `MissingRequiredRuntimeService` if the service cannot be found.
    {
        provider.template ensure_service<ServiceT>(name)
    } -> std::same_as<ServiceT &>;

    // Shio: Creates a default-constructed service instance.
    {
        provider.template create_default_service<ServiceT>(name)
    } -> std::convertible_to<std::expected<
        std::reference_wrapper<ServiceT>, ServiceProviderErrorCodes
    >>;

    // Shio: Takes ownership of a pre-made service instance.
    {
        provider.template create_service<ServiceT>(name, std::move(instance))
    } -> std::convertible_to<std::expected<
        std::reference_wrapper<ServiceT>, ServiceProviderErrorCodes
    >>;

    // Shio: Whether the service provider is thread-safe.
    { ProviderT::is_thread_safe_v } -> std::convertible_to<bool>;
    { ProviderT::is_thread_safe() } -> std::same_as<bool>;
} && type_traits::Movable<ProviderT>;

namespace details
{
template <typename ProviderT, typename ServiceT = int>
concept NamelessServiceProviderRequirements =
    requires(ProviderT provider, std::unique_ptr<ServiceT> instance) {
        {
            provider.template try_get_service<ServiceT>()
        } -> std::convertible_to<std::expected<
            std::reference_wrapper<ServiceT>, ServiceProviderErrorCodes
        >>;
        {
            provider.template ensure_service<ServiceT>()
        } -> std::same_as<ServiceT &>;
        {
            provider.template create_default_service<ServiceT>()
        } -> std::convertible_to<std::expected<
            std::reference_wrapper<ServiceT>, ServiceProviderErrorCodes
        >>;
        {
            provider.template create_service<ServiceT>(std::move(instance))
        } -> std::convertible_to<std::expected<
            std::reference_wrapper<ServiceT>, ServiceProviderErrorCodes
        >>;
        {
            provider.template generate_service_name<ServiceT>()
        } -> std::same_as<std::string>;
    };
} // namespace details

template <typename ProviderT, typename ServiceT = int>
concept NamelessServiceProvider = ServiceProvider<ProviderT, ServiceT> &&
    details::NamelessServiceProviderRequirements<ProviderT, ServiceT>;
} // namespace usagi::runtime
