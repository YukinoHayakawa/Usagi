#include <gtest/gtest.h>

#include <Usagi/Criteria/FunctorCriteria.hpp>
#include <Usagi/Criteria/LinkedCriteria.hpp>

/*

struct LambdaTest
{
std::function<bool(const int &i)> t1 { [](const int &i) {
return i == 0;
} };
std::function<bool(const int &i)> t2 { [](const int &i) {
return i >= 1;
} };
std::function<bool(const int &i)> t3 { [](const int &i) {
return i != 1;
} };
};

TEST(lambda, wtf)
{
LambdaTest t;

EXPECT_TRUE(t.t1(0));
EXPECT_FALSE(t.t1(1));

EXPECT_TRUE(t.t2(1));
EXPECT_TRUE(t.t2(2));
EXPECT_FALSE(t.t2(0));

EXPECT_TRUE(t.t3(0));
EXPECT_TRUE(t.t3(-1));
EXPECT_FALSE(t.t3(1));
}

*/

class LinkedCriteriaTest : public ::testing::Test
{
protected:
    int object = 0;

    std::shared_ptr<yuki::Criteria<int>> c1;
    std::shared_ptr<yuki::Criteria<int>> c2;
    std::shared_ptr<yuki::Criteria<int>> c3;

    /* this leads to strange test failures with clang/c2 where c1->isEligible(1) passes and 0 fails.
    std::shared_ptr<yuki::Criteria<int>> c1 { std::make_shared<yuki::FunctorCriteria<int>>([](const int &i) {
    return i == 0;
    }) };
    std::shared_ptr<yuki::Criteria<int>> c2 { std::make_shared<yuki::FunctorCriteria<int>>([](const int &i) {
    return i >= 1;
    }) };
    std::shared_ptr<yuki::Criteria<int>> c3 { std::make_shared<yuki::FunctorCriteria<int>>([](const int &i) {
    return i != 1;
    }) };
    */

    LinkedCriteriaTest()
        : c1 { std::make_shared<yuki::FunctorCriteria<int>>([](const int &i) { return i == 0; }) }
        , c2 { std::make_shared<yuki::FunctorCriteria<int>>([](const int &i) { return i >= 1; }) }
        , c3 { std::make_shared<yuki::FunctorCriteria<int>>([](const int &i) { return i != 1; }) }
    {
    }
};

TEST_F(LinkedCriteriaTest, criterion_are_correct)
{
    EXPECT_TRUE(c1->isEligible(0));
    EXPECT_FALSE(c1->isEligible(1));

    EXPECT_TRUE(c2->isEligible(1));
    EXPECT_TRUE(c2->isEligible(2));
    EXPECT_FALSE(c2->isEligible(0));

    EXPECT_TRUE(c3->isEligible(0));
    EXPECT_TRUE(c3->isEligible(-1));
    EXPECT_FALSE(c3->isEligible(1));
}

TEST_F(LinkedCriteriaTest, no_next)
{
    yuki::LinkedCriteria<int> l { c1 };

    EXPECT_TRUE(l.isEligible(object));
}

TEST_F(LinkedCriteriaTest, next_fail)
{
    yuki::LinkedCriteria<int> l { c1, c2.get() };

    EXPECT_FALSE(l.isEligible(object));
}

TEST_F(LinkedCriteriaTest, next_also_pass)
{
    yuki::LinkedCriteria<int> l { c1, c3.get() };

    EXPECT_TRUE(l.isEligible(object));
}
