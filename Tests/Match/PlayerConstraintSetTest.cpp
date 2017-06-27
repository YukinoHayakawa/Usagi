#include <gtest/gtest.h>

#include <Usagi/Match/Constraint/PlayerConstraintSet.hpp>
#include <Usagi/Match/Constraint/PlayerPropertyConstraint.hpp>

using namespace yuki::match;

class PlayerConstraintSetTest : public ::testing::Test
{
protected:
    PlayerConstraintSet s;
    Player p;

    PlayerConstraintSetTest()
    {
        p.setProperty("rank", 1);
        p.setProperty("os", "iOS");
    }
};

TEST_F(PlayerConstraintSetTest, all_pass)
{
    s.addConstraint(std::make_shared<PlayerPropertyConstraint<int, std::greater<int>>>("rank", 0));
    s.addConstraint(std::make_shared<PlayerPropertyConstraint<std::string, std::not_equal_to<std::string>>>("os", "Android"));

    EXPECT_TRUE(s.isPlayerEligible(p));
}

TEST_F(PlayerConstraintSetTest, one_fail)
{
    s.addConstraint(std::make_shared<PlayerPropertyConstraint<int, std::equal_to<int>>>("rank", 0));
    s.addConstraint(std::make_shared<PlayerPropertyConstraint<std::string, std::not_equal_to<std::string>>>("os", "Android"));

    EXPECT_FALSE(s.isPlayerEligible(p));
}
