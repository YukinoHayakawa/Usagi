#pragma once

#include <Usagi/Library/Values/Optional.hpp>

namespace usagi
{
/**
 * \brief Exception thrown when an operation on a `RawHandleResource` is
 *        attempted but the underlying resource handle is not present.
 * \details Shio: This typically occurs if you try to access the handle of a
 *          default-constructed, moved-from, or explicitly cleared resource
 *          wrapper. It's a specific type of `BadOptionalAccess` indicating a
 *          logic error in the resource's lifecycle management.
 */
class MissingManagedResource : public BadOptionalAccess
{
public:
    using BadOptionalAccess::BadOptionalAccess;

    /**
     * \brief Constructs the exception with a formatted message.
     */
    template <typename... Args>
    explicit MissingManagedResource(
        std::format_string<Args...> fmt, Args &&... args)
        : BadOptionalAccess(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};
} // namespace usagi
