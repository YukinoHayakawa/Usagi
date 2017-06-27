#include <gtest/gtest.h>
#include <memory>
#include <functional>

#include <Usagi/Match/Constraint/PlayerPropertyConstraint.hpp>
#include <Usagi/Match/Constraint/LinkedPlayerConstraint.hpp>

class LinkedPlayerConstriantTest : public ::testing::Test
{
protected:
    yuki::match::Player p;
    std::shared_ptr<yuki::match::PlayerConstriant> c1 { std::make_shared<yuki::match::PlayerPropertyConstraint<int, std::equal_to<int>>>("rank", 1) };
    std::shared_ptr<yuki::match::PlayerConstriant> c2 { std::make_shared<yuki::match::PlayerPropertyConstraint<std::string, std::equal_to<std::string>>>("os", "Android") };
    std::shared_ptr<yuki::match::PlayerConstriant> c3 { std::make_shared<yuki::match::PlayerPropertyConstraint<std::string, std::not_equal_to<std::string>>>("os", "Android") };

    LinkedPlayerConstriantTest()
    {
        p.setProperty("rank", 1);
        p.setProperty("os", "iOS");
    }
};

TEST_F(LinkedPlayerConstriantTest, no_next)
{
    yuki::match::LinkedPlayerConstriant l { c1 };

    EXPECT_TRUE(l.isPlayerEligible(p));
}

TEST_F(LinkedPlayerConstriantTest, next_fail)
{
    yuki::match::LinkedPlayerConstriant l { c1, c2.get() };

    EXPECT_FALSE(l.isPlayerEligible(p));
}

TEST_F(LinkedPlayerConstriantTest, next_also_pass)
{
    yuki::match::LinkedPlayerConstriant l { c1, c3.get() };

    EXPECT_TRUE(l.isPlayerEligible(p));
}
