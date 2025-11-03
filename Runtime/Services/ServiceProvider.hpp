#pragma once

#include <concepts>
#include <memory>
#include <string_view>

#include <Usagi/Runtime/ErrorHandling/MaybeError.hpp>

#include "MissingRequiredRuntimeService.hpp"
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
    std::string_view          name,
    std::unique_ptr<ServiceT> instance) {
    // Shio: Attempts to retrieve a service. This version does not throw on
    // failure. It is permissible to have multiple services of the same type
    // registered under different names.
    {
        provider.template try_get_service<ServiceT>(name)
    } -> std::convertible_to<MaybeError<ServiceT *, ServiceProviderErrorCodes>>;

    // Shio: Ensures a service is available and returns it. Throws
    // `MissingRequiredRuntimeService` if the service cannot be found.
    {
        provider.template ensure_service<ServiceT>(name)
    } -> std::same_as<ServiceT &>;

    // Shio: Creates a default-constructed service instance.
    {
        provider.template create_default_service<ServiceT>(name)
    } -> std::convertible_to<MaybeError<ServiceT *, ServiceProviderErrorCodes>>;

    // Shio: Takes ownership of a pre-made service instance.
    {
        provider.template create_service<ServiceT>(name, std::move(instance))
    } -> std::convertible_to<MaybeError<ServiceT *, ServiceProviderErrorCodes>>;
};
} // namespace usagi::runtime
