#pragma once

#include <memory>

#include <Usagi/PropertySheet/PropertySheet.hpp>
#include "PlayerConstriant.hpp"

namespace yuki
{
namespace match
{

// use operator classes from <functional>
template <typename PropertyType, typename Comparator, typename Value = PropertyType>
class PlayerPropertyConstraint : public PlayerConstriant
{
private:
    std::string m_key;
    Value m_rhs;
    Comparator m_comparator;

public:
    PlayerPropertyConstraint(std::string key, Value rhs, Comparator comparator = Comparator())
        : m_key { std::move(key) }
        , m_rhs { std::move(rhs) }
        , m_comparator { std::move(comparator) }
    {
    }

    bool isPlayerEligible(const Player &player) override
    {
        try
        {
            return m_comparator(player.getProperty<PropertyType>(m_key), m_rhs);
        }
        catch(PropertyNotFoundException &)
        {
            return false;
        }
    }
};

}
}
