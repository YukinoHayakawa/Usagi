#pragma once

#include <any>
#include <memory>
#include <string>
#include <unordered_map>

#include <Usagi/Library/Memory/Noncopyable.hpp>

#include "ServiceProvider.hpp"

namespace usagi::runtime
{
/**
 * \brief A simple implementation of the `ServiceProvider` concept.
 *
 * Shio: This class uses a `std::unordered_map` with `std::any` to store
 * service instances. This allows for storing services of any type under a
 * string name, with runtime type checking to ensure type safety.
 */
class SimpleRuntimeServiceProvider : Noncopyable
{
    std::unordered_map<std::string, std::any> mServices;

public:
    template <typename ServiceT>
    MaybeError<ServiceT *, ServiceProviderErrorCodes>
        try_get_service(std::string_view service_name)
    {
        const auto it = mServices.find(std::string(service_name));
        if(it == mServices.end())
        {
            return ServiceProviderErrorCodes::ServiceNotFound;
        }
        try
        {
            return std::any_cast<ServiceT *>(it->second);
        }
        catch(const std::bad_any_cast &)
        {
            return ServiceProviderErrorCodes::MismatchedServiceType;
        }
    }

    template <typename ServiceT>
    ServiceT &ensure_service(std::string_view service_name)
    {
        auto result = try_get_service<ServiceT>(service_name);
        if(result.has_error())
        {
            throw MissingRequiredRuntimeService(std::string(service_name));
        }
        return *result.value();
    }

    template <typename ServiceT>
    MaybeError<ServiceT *, ServiceProviderErrorCodes>
        create_default_service(std::string_view service_name)
    {
        return create_service<ServiceT>(
            service_name,
            std::make_unique<ServiceT>());
    }

    template <typename ServiceT>
    MaybeError<ServiceT *, ServiceProviderErrorCodes> create_service(
        std::string_view          service_name,
        std::unique_ptr<ServiceT> instance)
    {
        if(!instance)
        {
            return ServiceProviderErrorCodes::NullServiceInstance;
        }
        auto [it, inserted] =
            mServices.try_emplace(std::string(service_name), instance.get());
        if(!inserted)
        {
            return ServiceProviderErrorCodes::ServiceNameInUse;
        }
        // Shio: The user of this class is responsible for managing the
        // lifetime of the service instance. We release the unique_ptr here
        // to transfer ownership to the caller, who must ensure the service
        // outlives the provider.
        instance.release();
        return static_cast<ServiceT *>(it->second);
    }
};

static_assert(ServiceProvider<SimpleRuntimeServiceProvider>);
} // namespace usagi::runtime
