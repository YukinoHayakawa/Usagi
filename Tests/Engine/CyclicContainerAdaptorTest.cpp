#include <gtest/gtest.h>

#include <vector>

#include <Usagi/Engine/Utility/CyclicContainerAdaptor.hpp>

class CyclicContainerAdaptorTest : public ::testing::Test {
 protected:
    std::vector<int> original { 0, 1, 2, 3 };
    yuki::utility::CyclicContainerAdaptor<decltype(original)> adaptor
        { original, original.begin() + 2, 2 };
};

TEST_F(CyclicContainerAdaptorTest, Increment)
{
    std::vector<int> repeated;
    const std::vector<int> expected { 2, 3, 0, 1, 2, 3, 0, 1 };

    for(auto &i : adaptor)
    {
        repeated.push_back(i);
    }

    ASSERT_EQ(repeated, expected);
}

TEST_F(CyclicContainerAdaptorTest, Decrement) {
    std::vector<int> repeated;
    const std::vector<int> expected { 2, 3, 0, 1, 2, 3, 0, 1 };

    for(auto iter = adaptor.begin(); iter != adaptor.end(); ++iter)
    {
        ++iter; --iter;
        repeated.push_back(*iter);
    }

    ASSERT_EQ(repeated, expected);
}

TEST_F(CyclicContainerAdaptorTest, WrapCallback) {
    int counter = 0;
    auto iter = adaptor.begin();
    iter.setWrapCallback([&](int i) { counter += i; });
    ASSERT_EQ(counter, 0);
    ++iter; // 3
    ASSERT_EQ(counter, 0);
    ++iter; // 0
    ASSERT_EQ(counter, 1);
    --iter; // 3
    ASSERT_EQ(counter, 0);
}
