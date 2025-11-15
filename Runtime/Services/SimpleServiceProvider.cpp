#include "SimpleServiceProvider.hpp"

#include <shared_mutex>
#include <string>

namespace usagi::runtime
{
SimpleServiceProvider::SimpleServiceProvider()
    : RawHandleResource<std::shared_mutex *>(
        []() { return new std::shared_mutex(); },
        [](auto * m) { delete m; }
    )
{
}

SimpleServiceProvider::AnyAndSharedLock
SimpleServiceProvider::try_get_service_impl_locked(
    const std::string_view service_name
)
{
    std::shared_lock lock(*GetRawHandle());
    const auto       it = mServices.find(std::string(service_name));
    if(it == mServices.end())
    {
        return { nullptr, std::move(lock) };
    }
    return { &it->second, std::move(lock) };
}

bool SimpleServiceProvider::create_service_impl(
    const std::string_view service_name, std::any instance
)
{
    std::unique_lock lock(*GetRawHandle());
    const auto [it, inserted] =
        mServices.try_emplace(std::string(service_name), std::move(instance));
    return inserted;
}
} // namespace usagi::runtime
