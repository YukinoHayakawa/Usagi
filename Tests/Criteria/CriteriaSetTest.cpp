#include <gtest/gtest.h>

#include <Usagi/Criteria/CriteriaSet.hpp>
#include <Usagi/Criteria/FunctorCriteria.hpp>

using namespace yuki;

class CriteriaSetTest : public ::testing::Test
{
protected:
    CriteriaSet<int> s;
    int p = 0;

    CriteriaSetTest()
    {
    }
};

TEST_F(CriteriaSetTest, all_pass)
{
    s.addConstraint(std::make_shared<FunctorCriteria<int>>([](const int &i) { return i == 0; }));
    s.addConstraint(std::make_shared<FunctorCriteria<int>>([](const int &i) { return i != 1; }));

    EXPECT_TRUE(s.isEligible(p));
}

TEST_F(CriteriaSetTest, one_fail)
{
    s.addConstraint(std::make_shared<FunctorCriteria<int>>([](const int &i) { return i == 0; }));
    s.addConstraint(std::make_shared<FunctorCriteria<int>>([](const int &i) { return i == 1; }));

    EXPECT_FALSE(s.isEligible(p));
}
