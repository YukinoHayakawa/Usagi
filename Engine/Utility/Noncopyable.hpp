#pragma once

namespace yuki
{
/**
 * \brief Note that classes deriving from a class with deleted special
 * member functions will not implicitly generate them either.
 */
class Noncopyable
{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable &other) = delete;
    Noncopyable & operator=(const Noncopyable &other) = delete;
};
}
