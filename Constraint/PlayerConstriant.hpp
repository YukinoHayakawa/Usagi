#pragma once

namespace yuki
{
namespace match
{

class Player;

class PlayerConstriant
{
public:
    virtual ~PlayerConstriant()
    {
    }

    virtual bool isPlayerEligible(const Player &player) = 0;
};

}
}
