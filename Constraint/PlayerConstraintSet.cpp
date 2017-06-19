#include "PlayerConstraintSet.hpp"

void yuki::match::PlayerConstraintSet::addConstraint(std::shared_ptr<PlayerConstriant> constraint)
{
    m_constriants.push_back(std::move(constraint));
}

bool yuki::match::PlayerConstraintSet::isPlayerEligible(const Player &player)
{
    for(auto &&c : m_constriants)
    {
        if(!c->isPlayerEligible(player)) return false;
    }
    return true;
}

bool yuki::match::PlayerConstraintSet::isPlayerEligible(const Player &player, FailureCallback failure_callback)
{
    bool hasFailed = false;
    for(auto &&c : m_constriants)
    {
        if(!c->isPlayerEligible(player))
        {
            failure_callback(*c.get());
            hasFailed = true;
        }
    }
    return !hasFailed;
}
