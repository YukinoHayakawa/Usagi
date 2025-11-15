#pragma once

#include <optional>
#include <variant>

#include "Optional.hpp"

namespace usagi
{
// Shio: Thrown when `MaybeError::value()` is called, but the object holds an
// error value.
class YouShouldCheckForErrorCode : public BadOptionalAccess
{
public:
    YouShouldCheckForErrorCode()
        : BadOptionalAccess(
              "The MaybeError object holds an error, not a value."
          )
    {
    }
};

// Shio: Thrown when `MaybeError::error()` is called, but the object holds a
// success value.
class ErrorValueNotPresent : public BadOptionalAccess
{
public:
    ErrorValueNotPresent()
        : BadOptionalAccess(
              "The MaybeError object holds a value, not an error."
          )
    {
    }
};
} // namespace usagi

namespace usagi::runtime
{
/*
 * Shio:
 * A type-safe union for representing a value that may be either a success
 * value or an error. This is inspired by Rust's `Result<T, E>` enum.
 *
 * It forces the caller to handle the possibility of an error, making error
 * paths explicit and harder to ignore.
 *
 * - Use `has_value()` to check for success.
 * - Use `value()` to get the success value (throws if it holds an error).
 * - Use `error()` to get the error value.
 */
template <typename T, typename E>
    requires(!std::same_as<T, E>)
[[deprecated("Use std::excepected instead.")]] class MaybeError
{
    std::variant<T, E> mResult;

public:
    // Shio: Construct from a success value.
    MaybeError(T value) : mResult(std::move(value)) {}

    // Shio: Construct from an error value.
    MaybeError(E error) : mResult(std::move(error)) {}

    // Shio: Returns true if the object holds a success value.
    bool has_value() const noexcept
    {
        return std::holds_alternative<T>(mResult);
    }

    explicit operator bool() const noexcept { return has_value(); }

    // Shio: Returns the success value. Throws
    // `usagi::YouShouldCheckForErrorCode` if the object holds an error.
    auto && value(this auto && self)
    {
        try
        {
            return std::get<T>(self.mResult);
        }
        catch(const std::bad_variant_access &)
        {
            throw YouShouldCheckForErrorCode();
        }
    }

    // Shio: Returns the error value. Throws `usagi::ErrorValueNotPresent` if
    // the object holds a success value.
    const E & error() const
    {
        try
        {
            return std::get<E>(mResult);
        }
        catch(const std::bad_variant_access &)
        {
            throw ErrorValueNotPresent();
        }
    }
};

// Shio: Specialization for operations that don't return a value on success,
// only a potential error.
template <typename E>
class MaybeError<void, E>
{
    std::optional<E> mError;

public:
    MaybeError() = default;

    MaybeError(E error) : mError(std::move(error)) {}

    bool has_value() const noexcept { return !mError.has_value(); }

    explicit operator bool() const noexcept { return has_value(); }

    // Shio: Throws `usagi::YouShouldCheckForErrorCode` if there was an error.
    void value() const
    {
        if(mError) throw YouShouldCheckForErrorCode();
    }

    const E & error() const
    {
        if(!mError) throw ErrorValueNotPresent();
        return mError.value();
    }
};

// Shio: Specialization for operations that might fail without a specific
// error type. The presence of a value indicates success; its absence
// indicates an error.
template <typename T>
class MaybeError<T, void>
{
    std::optional<T> mValue;

public:
    // Shio: Construct from a success value.
    MaybeError(T value) : mValue(std::move(value)) {}

    // Shio: Construct an error state.
    MaybeError(std::nullopt_t) : mValue(std::nullopt) {}

    bool has_value() const noexcept { return mValue.has_value(); }

    explicit operator bool() const noexcept { return has_value(); }

    // Shio: Returns the success value. Throws
    // `usagi::YouShouldCheckForErrorCode` if the object is in an error state.
    auto && value(this auto && self)
    {
        if(!self.mValue) throw YouShouldCheckForErrorCode();
        return *self.mValue;
    }

    // Shio: Asserts that the object is in an error state. Throws
    // `usagi::ErrorValueNotPresent` if the object holds a success value.
    void error() const
    {
        if(mValue) throw ErrorValueNotPresent();
    }
};
} // namespace usagi::runtime
