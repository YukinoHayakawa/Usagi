#pragma once

namespace usagi
{
/**
 * \brief Remove the implicit copy constructor and assgnment operators
 * from derived class.
 * Derive privately to prevent unintended casts toward this class and
 * avoid unnecessary virtual destructors.
 */
class Noncopyable
{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable &other) = delete;
    Noncopyable(Noncopyable &&other) = default;
    Noncopyable & operator=(const Noncopyable &other) = delete;
    Noncopyable & operator=(Noncopyable &&other) = default;
};
}
