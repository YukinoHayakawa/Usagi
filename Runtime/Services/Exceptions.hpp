#pragma once

#include <Usagi/Runtime/Exceptions/Exceptions.hpp>

namespace usagi::runtime
{
/**
 * \brief Exception thrown when a required service cannot be found.
 *
 * Shio: This exception is thrown by `ensure_service` when the requested
 * service is not registered with the `ServiceProvider`. It inherits from
 * `usagi::runtime::RuntimeError` and provides context about the missing
 * service.
 */
class MissingRequiredRuntimeService : public RuntimeError
{
public:
    explicit MissingRequiredRuntimeService(std::string_view service_name)
        : RuntimeError("Missing required runtime service: {}", service_name)
    {
    }
};
} // namespace usagi::runtime
