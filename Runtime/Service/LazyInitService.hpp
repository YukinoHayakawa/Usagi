#pragma once

#include <memory>

#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
template <typename ServiceT>
struct LazyInitService : Noncopyable
{
    using ServiceT = ServiceT;

    ServiceT &get_service()
    {
        if(!mServiceImpl)
        {
            mServiceImpl = std::make_unique<ServiceT>();
        }
        return *mServiceImpl;
    }

private:
    std::unique_ptr<ServiceT> mServiceImpl;
};
}
