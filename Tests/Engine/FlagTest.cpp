#include "gtest/gtest.h"

#include <Usagi/Engine/Utility/Flag.hpp>

TEST(FlagTest, MatchAll) {
    EXPECT_TRUE(yuki::utility::matchAllFlags(0b0011, 0b0010));
    EXPECT_TRUE(yuki::utility::matchAllFlags(0b0011, 0b0011));
    EXPECT_FALSE(yuki::utility::matchAllFlags(0b0011, 0b0100));
    EXPECT_FALSE(yuki::utility::matchAllFlags(0b0011, 0b1100));
    EXPECT_FALSE(yuki::utility::matchAllFlags(0b0011, 0b1111));
    EXPECT_FALSE(yuki::utility::matchAllFlags(0b0011, 0b1001));
}
