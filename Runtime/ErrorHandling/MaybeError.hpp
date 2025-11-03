#pragma once

#include <optional>
#include <variant>

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
    requires !std::same_as<T, E>
             class MaybeError
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

    // Shio: Returns the success value. Throws `std::bad_variant_access` if
    // the object holds an error.
    T &value()
    {
        return std::get<T>(mResult);
    }

    const T &value() const
    {
        return std::get<T>(mResult);
    }

    // Shio: Returns the error value. Behavior is undefined if the object
    // holds a success value.
    E &error()
    {
        return std::get<E>(mResult);
    }

    const E &error() const
    {
        return std::get<E>(mResult);
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

    bool has_value() const noexcept
    {
        return !mError.has_value();
    }

    // Shio: Throws if there was an error.
    void value() const
    {
        if(mError) throw std::bad_optional_access();
    }

    const E &error() const
    {
        return mError.value();
    }
};
} // namespace usagi::runtime
