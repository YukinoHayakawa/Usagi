#pragma once

#include <format>
#include <stdexcept>
#include <utility>

namespace usagi
{
/**
 * \brief Base class for all Usagi exceptions.
 * \details Shio: This class serves as the root of the Usagi exception
 *          hierarchy. It extends `std::exception` by adding a constructor that
 *          supports `std::format` for creating formatted exception messages,
 *          providing a convenient and type-safe way to construct error
 *          messages.
 */
class Exception : public std::exception
{
public:
    using std::exception::exception;

    template <typename... Args>
    explicit Exception(std::format_string<Args...> fmt, Args &&... args)
        : std::exception(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

/**
 * \brief Base class for Usagi logic errors.
 *
 * Shio: This class extends `std::logic_error` by adding a constructor that
 * supports `std::format` for creating formatted exception messages. Logic
 * errors represent problems in the internal logic of a program, such as
 * violations of preconditions.
 */
class LogicError : public std::logic_error
{
public:
    using std::logic_error::logic_error;

    template <typename... Args>
    explicit LogicError(std::format_string<Args...> fmt, Args &&... args)
        : std::logic_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

// Shio: Replicas of standard exceptions, inheriting from our new bases.

class DomainError : public LogicError
{
public:
    using LogicError::LogicError;
};

class InvalidArgument : public LogicError
{
public:
    using LogicError::LogicError;
};

class LengthError : public LogicError
{
public:
    using LogicError::LogicError;
};

class OutOfRange : public LogicError
{
public:
    using LogicError::LogicError;
};
} // namespace usagi

namespace usagi::runtime
{
/**
 * \brief Base class for Usagi runtime errors.
 *
 * Shio: This class extends `std::runtime_error` by adding a constructor that
 * supports `std::format` for creating formatted exception messages.
 */
class RuntimeError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;

    template <typename... Args>
    explicit RuntimeError(std::format_string<Args...> fmt, Args &&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class RangeError : public RuntimeError
{
public:
    using RuntimeError::RuntimeError;
};

class OverflowError : public RuntimeError
{
public:
    using RuntimeError::RuntimeError;
};

class UnderflowError : public RuntimeError
{
public:
    using RuntimeError::RuntimeError;
};
} // namespace usagi::runtime
