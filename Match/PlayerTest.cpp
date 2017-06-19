#include <gtest/gtest.h>
#include "Usagi/Match/Player.hpp"

class PlayerTest : public ::testing::Test
{
protected:
    yuki::match::Player p;
};

TEST_F(PlayerTest, update_property)
{
    p.setProperty("rank", 1);
    EXPECT_EQ(p.getProperty<int>("rank"), 1);
    p.setProperty("rank", 2);
    EXPECT_EQ(p.getProperty<int>("rank"), 2);
}

TEST_F(PlayerTest, throw_if_not_found)
{
    EXPECT_THROW(p.getProperty<int>("rank"), yuki::match::PropertyNotFoundException);
}
