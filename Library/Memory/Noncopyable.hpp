#pragma once

namespace usagi
{
class Noncopyable
{
public:
    Noncopyable() = default;
    Noncopyable(const Noncopyable &other) = delete;
    Noncopyable(Noncopyable &&other) noexcept = default;
    Noncopyable & operator=(const Noncopyable &other) = delete;
    Noncopyable & operator=(Noncopyable &&other) noexcept = default;
};
}
