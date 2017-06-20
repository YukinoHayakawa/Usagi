#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Criteria.hpp"

namespace yuki
{

template <typename T>
class CriteriaSet : public Criteria<T>
{
private:
    typedef Criteria<T> constraint_t;

    std::vector<std::shared_ptr<constraint_t>> m_constriants;

public:
    void addConstraint(std::shared_ptr<constraint_t> constraint)
    {
        m_constriants.push_back(std::move(constraint));
    }

    bool isEligible(const T &target) override
    {
        for(auto &&c : m_constriants)
        {
            if(!c->isEligible(target)) return false;
        }
        return true;
    }

    typedef std::function<void(constraint_t &constriant)> FailureCallback;
    bool isEligible(const T &target, FailureCallback failure_callback)
    {
        bool hasFailed = false;
        for(auto &&c : m_constriants)
        {
            if(!c->isEligible(target))
            {
                failure_callback(*c.get());
                hasFailed = true;
            }
        }
        return !hasFailed;
    }
};

}
