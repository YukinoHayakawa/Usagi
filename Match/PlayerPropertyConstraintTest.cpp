#include <gtest/gtest.h>

#include <Usagi/Match/Player.hpp>
#include <Usagi/Match/Constraint/PlayerPropertyConstraint.hpp>

namespace
{

enum class PhoneOS
{
    iOS,
    Android,
};

enum class TXIM
{
    WeChat,
    QQ,
};

enum class 段位
{
    青铜,
    白银,
    黄金,
    铂金,
    钻石,
    王者,
};

}

using namespace yuki::match;

TEST(PlayerPropertyConstraintTest, default_to_not_eligible)
{
    PlayerPropertyConstraint<段位, std::equal_to<段位>> c("rank", 段位::钻石);

    Player p, p2;
    p.setProperty("rank", 段位::钻石);

    EXPECT_TRUE(c.isPlayerEligible(p));
    EXPECT_FALSE(c.isPlayerEligible(p2)); // p2 does not have such property

    p.setProperty("rank", 段位::王者);
    EXPECT_FALSE(c.isPlayerEligible(p)); // not equal
}
