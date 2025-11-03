#pragma once

#include <cstdint>

namespace usagi::runtime
{
/**
 * \brief Error codes for ServiceProvider operations.
 */
enum class ServiceProviderErrorCodes : std::uint8_t
{
    /**
     * \brief The requested service was not found in the provider.
     */
    ServiceNotFound,

    /**
     * \brief The service name exists, but corresponds to a different type.
     */
    MismatchedServiceType,

    /**
     * \brief The service name is already registered.
     */
    ServiceNameInUse,

    /**
     * \brief A null service instance was provided.
     */
    NullServiceInstance,
};
} // namespace usagi::runtime
