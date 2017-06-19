#pragma once

#include <memory>

#include "PlayerConstriant.hpp"

namespace yuki
{
namespace match
{

class LinkedPlayerConstriant : public PlayerConstriant
{
    std::shared_ptr<PlayerConstriant> m_this;
    PlayerConstriant *m_next = nullptr;
    
public:
    LinkedPlayerConstriant(std::shared_ptr<PlayerConstriant> player_constriant, PlayerConstriant *next = nullptr)
        : m_this { std::move(player_constriant) }
        , m_next { next }
    {
    }

    bool isPlayerEligible(const Player &player) override
    {
        return m_this->isPlayerEligible(player) && (!m_next || m_next->isPlayerEligible(player));
    }
};

}
}
