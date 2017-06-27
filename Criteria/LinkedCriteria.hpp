#pragma once

#include <memory>

#include "Criteria.hpp"

namespace yuki
{

template <typename T>
class LinkedCriteria : public Criteria<T>
{
    typedef Criteria<T> criteria_t;

    std::shared_ptr<criteria_t> m_this;
    criteria_t *m_next = nullptr;
    
public:
    LinkedCriteria(std::shared_ptr<criteria_t> criteria, criteria_t *next = nullptr)
        : m_this { std::move(criteria) }
        , m_next { next }
    {
    }

    bool isEligible(const T &target) override
    {
        return m_this->isEligible(target) && (m_next == nullptr || m_next->isEligible(target));
    }
};

}
