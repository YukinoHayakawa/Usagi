#pragma once

#include <type_traits>

namespace usagi
{
template <typename Service>
class ServiceAlias
{
    template <typename Runtime>
    explicit ServiceAlias(Runtime &runtime)
    {
    }

    static_assert(
        std::is_same_v<Service, Service> == false,
        "No alias accessor defined!"
    );
};

struct ServiceTest
{
    // ServiceTest & test_manager(); no not here
};


template <>
class ServiceAlias<ServiceTest>
{
    ServiceTest *mService;

public:
    template <typename Runtime>
    explicit ServiceAlias(Runtime &runtime)
        : mService(static_cast<ServiceTest *>(&runtime))
    {
    }

    ServiceTest & test_manager(); // exposed to user
}; // todo make a macro


/**
 * \brief Provides System with type information of services. App service set
 * is defined outside System implementation which causes the System unable to
 * know the exact type of the services set. This access interface serves as
 * an interface between the app runtime and the System.
 * \tparam Services Set of services used by the System.
 */
template <typename... Services>
class ServiceAccess : public ServiceAlias<Services>...
{
    // std::tuple<Services *...> mServices;

public:
    // Intentionally non-explicit: auto conversion from runtime type.
    template <typename Runtime>
    // ReSharper disable once CppNonExplicitConvertingConstructor
    ServiceAccess(Runtime &rt)
        : ServiceAlias<Services>(rt)...
    {
    }
};

inline void test(ServiceAccess<ServiceTest> rt)
{
    rt.test_manager();
}
}

/**
 * \brief Define an alias name for the specified service that to be used with
 * ServiceAccess. Once defined, the accessor name can be used by the System
 * to access the Service.
 * \param service_type 
 * \param accessor_name 
 */
#define USAGI_DECL_SERVICE_ALIAS(service_type, accessor_name) \
template <> class ServiceAlias<service_type> \
{ \
    using ServiceT = service_type; \
    ServiceT *mService = nullptr; \
public: \
    template <typename Runtime> \
    explicit ServiceAlias(Runtime &runtime) \
        : mService(static_cast<ServiceT *>(&runtime)) {} \
    /* todo remove get_service() */ \
    ServiceT::ServiceT & accessor_name() const \
        { return mService->get_service(); } \
} \
/**/
