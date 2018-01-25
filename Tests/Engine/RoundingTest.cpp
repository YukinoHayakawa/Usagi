#include "gtest/gtest.h"

#include <Usagi/Engine/Utility/Rounding.hpp>

TEST(RoundingTest, SpanningPages) {
    EXPECT_EQ(yuki::utility::calculateSpanningPages(0, 1), 0);
    EXPECT_EQ(yuki::utility::calculateSpanningPages(4, 4), 1);
    EXPECT_EQ(yuki::utility::calculateSpanningPages(16, 4), 4);
    EXPECT_EQ(yuki::utility::calculateSpanningPages(17, 4), 5);
}
