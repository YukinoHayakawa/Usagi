#pragma once

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
template <typename Create, typename Release>
class RAIIHelper : Noncopyable
{
    Create mCreate;
    Release mRelease;

public:
    RAIIHelper(Create create, Release release)
        : mCreate { std::move(create) }
        , mRelease { std::move(release) }
    {
        mCreate();
    }

    ~RAIIHelper()
    {
        mRelease();
    }
};
}
