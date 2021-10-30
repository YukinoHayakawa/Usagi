#pragma once

#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
template <typename T>
struct SimpleService : Noncopyable
{
    using ServiceT = T;

    ServiceT & get_service() { return mService; }

private:
    T mService;
};
}
