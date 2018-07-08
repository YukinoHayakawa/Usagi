#pragma once

#include <functional>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class RAIIHelper : Noncopyable
{
    std::function<void()> mCreate, mRelease;

public:
	RAIIHelper(std::function<void()> create, std::function<void()> release)
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
