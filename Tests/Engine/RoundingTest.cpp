#include "gtest/gtest.h"

#include <Usagi/Engine/Utility/Rounding.hpp>

TEST(RoundingTest, SpanningPages) {
    EXPECT_EQ(yuki::utility::calculateSpanningPages(0, 1), 0);
    EXPECT_EQ(yuki::utility::calculateSpanningPages(4, 4), 1);
    EXPECT_EQ(yuki::utility::calculateSpanningPages(16, 4), 4);
    EXPECT_EQ(yuki::utility::calculateSpanningPages(17, 4), 5);
}

TEST(RoundingTest, RoundUpUnsigned) {
    EXPECT_EQ(yuki::utility::roundUpUnsigned(0, 1), 0);
    EXPECT_EQ(yuki::utility::roundUpUnsigned(1, 1), 1);
    EXPECT_EQ(yuki::utility::roundUpUnsigned(1, 4), 4);
    EXPECT_EQ(yuki::utility::roundUpUnsigned(0, 16), 0);
    EXPECT_EQ(yuki::utility::roundUpUnsigned(3, 16), 16);
    EXPECT_EQ(yuki::utility::roundUpUnsigned(16, 16), 16);
}
