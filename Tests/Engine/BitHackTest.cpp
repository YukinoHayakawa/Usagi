#include "gtest/gtest.h"

#include <Usagi/Engine/Utility/BitHack.hpp>

TEST(BitHackTest, PowerOfTwoOrZero) {
    EXPECT_FALSE(yuki::utility::isPowerOfTwoOrZero(-2));
    EXPECT_FALSE(yuki::utility::isPowerOfTwoOrZero(-1));
    EXPECT_TRUE(yuki::utility::isPowerOfTwoOrZero(0));
    EXPECT_TRUE(yuki::utility::isPowerOfTwoOrZero(1));
    EXPECT_TRUE(yuki::utility::isPowerOfTwoOrZero(2));
    EXPECT_TRUE(yuki::utility::isPowerOfTwoOrZero(16));
    EXPECT_TRUE(yuki::utility::isPowerOfTwoOrZero(1024));
    EXPECT_FALSE(yuki::utility::isPowerOfTwoOrZero(12345));
}

TEST(BitHackTest, PowerOfTwo) {
    EXPECT_FALSE(yuki::utility::isPowerOfTwo(-2));
    EXPECT_FALSE(yuki::utility::isPowerOfTwo(-1));
    EXPECT_FALSE(yuki::utility::isPowerOfTwo(0));
    EXPECT_TRUE(yuki::utility::isPowerOfTwo(1));
    EXPECT_TRUE(yuki::utility::isPowerOfTwo(2));
    EXPECT_TRUE(yuki::utility::isPowerOfTwo(16));
    EXPECT_TRUE(yuki::utility::isPowerOfTwo(1024));
    EXPECT_FALSE(yuki::utility::isPowerOfTwo(12345));

}
