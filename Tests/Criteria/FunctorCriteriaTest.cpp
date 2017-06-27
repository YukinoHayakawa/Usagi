#include <gtest/gtest.h>

#include <Usagi/Criteria/FunctorCriteria.hpp>

class FunctorCriteriaTest : public ::testing::Test
{
protected:
    FunctorCriteriaTest()
    {
    }

    int o = 0;
};

TEST_F(FunctorCriteriaTest, pass)
{
    EXPECT_TRUE(yuki::FunctorCriteria<int>([](const int &i)
    {
        return i == 0;
    }).isEligible(o));
}

TEST_F(FunctorCriteriaTest, fail)
{
    EXPECT_FALSE(yuki::FunctorCriteria<int>([](const int &i)
    {
        return i == 1;
    }).isEligible(o));
}
