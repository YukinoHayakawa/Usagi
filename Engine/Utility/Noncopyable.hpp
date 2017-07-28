#pragma once

namespace yuki
{

class Noncopyable
{
public:
    Noncopyable() = default;
    Noncopyable(const Noncopyable &other) = delete;
    Noncopyable(Noncopyable &&other) noexcept = delete;
    Noncopyable & operator=(const Noncopyable &other) = delete;
    Noncopyable & operator=(Noncopyable &&other) noexcept = delete;
};

}
