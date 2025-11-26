#pragma once

#include <concepts>
#include <expected>
#include <functional>
#include <memory>
#include <string_view>
#include <variant>

#include <Usagi/Library/Meta/Types/Traits.hpp>

#include "ServiceProviderErrorCodes.hpp"

namespace usagi::runtime
{
struct UseDefaultServiceNameTag
{
};

constexpr inline UseDefaultServiceNameTag default_service_name;
using optional_service_name_t =
    std::variant<UseDefaultServiceNameTag, std::string_view>;

template <typename ServiceT>
using maybe_service_t =
    std::expected<std::reference_wrapper<ServiceT>, ServiceProviderErrorCodes>;

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
                              std::unique_ptr<ServiceT> instance
                          ) {
    // Shio: Attempts to retrieve a service. This version does not throw on
    // failure. It is permissible to have multiple services of the same type
    // registered under different names.
    {
        provider.template try_get_service<ServiceT>(name)
    } -> std::convertible_to<maybe_service_t<ServiceT>>;

    // Shio: Ensures a service is available and returns it. Throws
    // `MissingRequiredRuntimeService` if the service cannot be found.
    {
        provider.template ensure_service<ServiceT>(name)
    } -> std::same_as<ServiceT &>;

    // Shio: Creates a default-constructed service instance.
    {
        provider.template create_default_service<ServiceT>(name)
    } -> std::convertible_to<maybe_service_t<ServiceT>>;

    // Shio: Takes ownership of a pre-made service instance.
    {
        provider.template create_service<ServiceT>(std::move(instance), name)
    } -> std::convertible_to<maybe_service_t<ServiceT>>;

    // The ServiceProvider calls `std::make_unique` to create the service.
    // todo: support checking construction with actual ctor args
    {
        provider.template create_service_inplace<ServiceT>(name, 0)
    } -> std::convertible_to<maybe_service_t<ServiceT>>;

    // Shio: Whether the service provider is thread-safe.
    { ProviderT::is_thread_safe_v } -> std::convertible_to<bool>;
    { ProviderT::is_thread_safe() } -> std::same_as<bool>;

    // Shio: Whether the service provider supports polymorphic retrieval.
    // If true, it means that a service registered with a derived type can be
    // retrieved by requesting a public and unambiguous base type. For example,
    // if a `Cat` service is registered, `try_get_service<Animal>()` should
    // succeed, assuming `Cat` inherits from `Animal`.
    { ProviderT::is_polymorphic_v } -> std::convertible_to<bool>;
    { ProviderT::is_polymorphic() } -> std::same_as<bool>;
} && type_traits::Movable<ProviderT>;

namespace details
{
template <typename ProviderT, typename ServiceT = int>
concept NamelessServiceProviderRequirements = requires(
    ProviderT                 provider,
    optional_service_name_t   name,
    std::unique_ptr<ServiceT> instance
) {
    {
        provider.template try_get_service<ServiceT>()
    } -> std::convertible_to<maybe_service_t<ServiceT>>;
    {
        provider.template ensure_service<ServiceT>()
    } -> std::same_as<ServiceT &>;
    {
        provider.template create_default_service<ServiceT>()
    } -> std::convertible_to<maybe_service_t<ServiceT>>;
    {
        provider.template create_service<ServiceT>(std::move(instance))
    } -> std::convertible_to<maybe_service_t<ServiceT>>;
    {
        provider.template generate_service_name<ServiceT>()
    } -> std::convertible_to<std::string_view>;
    // todo: support checking construction with actual ctor args
    {
        provider.template create_service_inplace<ServiceT>(0)
    } -> std::convertible_to<maybe_service_t<ServiceT>>;
};
} // namespace details

template <typename ProviderT, typename ServiceT = int>
concept NamelessServiceProvider = ServiceProvider<ProviderT, ServiceT> &&
    details::NamelessServiceProviderRequirements<ProviderT, ServiceT>;
} // namespace usagi::runtime
