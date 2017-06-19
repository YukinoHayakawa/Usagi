#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "PlayerConstriant.hpp"

namespace yuki
{
namespace match
{

class PlayerConstraintSet : public PlayerConstriant
{
private:
    std::vector<std::shared_ptr<PlayerConstriant>> m_constriants;

public:
    void addConstraint(std::shared_ptr<PlayerConstriant> constraint);

    bool isPlayerEligible(const Player &player) override;

    typedef std::function<void(PlayerConstriant &constriant)> FailureCallback;
    bool isPlayerEligible(const Player &player, FailureCallback failure_callback);
};

}
}
