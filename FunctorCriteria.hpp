#pragma once

#include <functional>

#include "Criteria.hpp"

namespace yuki
{

template <typename T>
class FunctorCriteria : public Criteria<T>
{
    typedef std::function<bool(const T &target)> functor_t;
    functor_t m_functor;

public:
    FunctorCriteria(functor_t functor)
        : m_functor { std::move(functor) }
    {
    }

    bool isEligible(const T &target) override
    {
        return m_functor(target);
    }
};

}
