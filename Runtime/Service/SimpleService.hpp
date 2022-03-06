#pragma once

#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
template <typename T>
struct SimpleService : Noncopyable
{
    using ServiceT = T;

    ServiceT & get_service() { return mService; }

    template <typename... Args>
    explicit SimpleService(Args &&... args)
        : mService(std::forward<Args>(args)...)
    {
    }

private:
    T mService;
};
}
