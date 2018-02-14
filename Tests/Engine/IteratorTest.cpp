#include "gtest/gtest.h"

#include <Usagi/Engine/Utility/Iterator.h>

using namespace yuki::utility;

class FindFirstConsecutiveTest : public ::testing::Test
{
protected:
    FindFirstConsecutiveTest() = default;

    std::vector<int> a { 0, 1, 2, 3, 3, 4, 3, 3, 3, 5, 3, 3, 3, 3, 3 };
};

TEST_F(FindFirstConsecutiveTest, OnlyFindsTheFirst)
{
    EXPECT_EQ(findFirstConsecutive(a.begin(), a.end(), 3, 3), a.begin() + 6);
}

TEST_F(FindFirstConsecutiveTest, DoesNotPassTheEnd)
{
    EXPECT_EQ(findFirstConsecutive(a.begin(), a.begin() + 7, 3, 3), a.begin() + 7);
}

TEST_F(FindFirstConsecutiveTest, NotLongEnough)
{
    EXPECT_EQ(findFirstConsecutive(a.begin(), a.end(), 3, 6), a.end());
}

TEST_F(FindFirstConsecutiveTest, Nonexistent)
{
    EXPECT_EQ(findFirstConsecutive(a.begin(), a.end(), 10, 10), a.end());
}

TEST_F(FindFirstConsecutiveTest, StartFromMiddle)
{
    EXPECT_EQ(findFirstConsecutive(a.end() - 4, a.end(), 3, 4), a.end() - 4);
}

TEST_F(FindFirstConsecutiveTest, FindSingle)
{
    EXPECT_EQ(findFirstConsecutive(a.begin(), a.end(), 0, 1), a.begin());
}

TEST_F(FindFirstConsecutiveTest, FindZeroCount)
{
    EXPECT_EQ(findFirstConsecutive(a.begin(), a.end(), 0, 0), a.begin());
    EXPECT_EQ(findFirstConsecutive(a.begin(), a.end(), 16, 0), a.begin());
}
