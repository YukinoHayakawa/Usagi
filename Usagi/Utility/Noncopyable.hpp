#pragma once

namespace usagi
{
/**
 * \brief Remove the implicit copy constructor and assignment operators
 * from derived class.
 * Derive privately to prevent unintended casts toward this class and
 * avoid unnecessary virtual destructors.
 */
class Noncopyable
{
public:
    Noncopyable() = default;
    ~Noncopyable() noexcept = default;
    Noncopyable(const Noncopyable &other) = delete;
    Noncopyable(Noncopyable &&other) noexcept = default;
    Noncopyable & operator=(const Noncopyable &other) = delete;
    Noncopyable & operator=(Noncopyable &&other) noexcept = default;
};
}
