#pragma once

#include <memory>

namespace usagi
{
template <typename ServiceT>
struct LazyInitService
{
    using ServiceT = ServiceT;

    ServiceT &get_service()
    {
        if(!mServiceImpl)
        {
            mServiceImpl = std::make_unique<ServiceT>();
        }
        return *mServiceImpl.get();
    }

private:
    std::unique_ptr<ServiceT> mServiceImpl;
};
}
