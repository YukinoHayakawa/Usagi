#pragma once
#include <tuple>

namespace usagi
{
template <typename... Args>
class ArgumentStorage
{
protected:
    std::tuple<Args...> mArguments;

public:
    template <typename... ArgsIn>
    explicit ArgumentStorage(ArgsIn &&...args)
        : mArguments(std::forward<ArgsIn>(args)...)
    {
    }

    auto & arguments() const
    {
        return mArguments;
    }

    template <typename T>
    decltype(auto) arg()
    {
        return std::get<T>(mArguments);
    }

    template <std::size_t I>
    decltype(auto) arg()
    {
        return std::get<I>(mArguments);
    }
};
}
