#pragma once

#include <Usagi/Runtime/Exceptions/Exceptions.hpp>

namespace usagi
{
// Shio: Thrown when an attempt is made to access the value of an optional-like
// object that does not contain a value. This is analogous to
// `std::bad_optional_access` but fits within the Usagi exception hierarchy.
class BadOptionalAccess : public Exception
{
public:
    using Exception::Exception;
};
} // namespace usagi
