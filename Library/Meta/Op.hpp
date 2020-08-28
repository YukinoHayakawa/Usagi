#pragma once

namespace usagi
{
template <typename Base>
struct OpDeref : Base
{
    using Base::Base;

    template <typename... Args>
    bool operator()(Args &&... args) const
    {
        return Base::operator()(*args...);
    }
};

template <typename Base, typename Eval>
struct OpEval : Base
{
    Eval eval;

    using Base::Base;

    OpEval(Eval eval)
        : eval(std::move(eval))
    {
    }

    template <typename... Args>
    bool operator()(Args &&... args) const
    {
        return Base::operator()(eval(std::forward<Args>(args))...);
    }
};

template <typename Base, std::size_t Index>
struct OpGet : Base
{
    using Base::Base;

    template <typename... Args>
    bool operator()(Args &&... args) const
    {
        return Base::operator()(std::get<Index>(std::forward<Args>(args))...);
    }
};
}
