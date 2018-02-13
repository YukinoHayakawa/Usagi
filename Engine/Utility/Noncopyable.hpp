#pragma once

namespace yuki
{
class Noncopyable
{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable &other) = delete;
    Noncopyable & operator=(const Noncopyable &other) = delete;
};
}
