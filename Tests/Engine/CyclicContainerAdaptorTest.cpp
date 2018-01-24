#include <gtest/gtest.h>

#include <vector>
#include "../Engine/Utility/CyclicContainerAdaptor.hpp"

TEST(CyclicContainerAdaptorTest, RemappedBeginAndEnd) {
    std::vector<int> original { 0, 1, 2, 3, 4, 5, 6 };
    std::vector<int> repeated;
    const std::vector<int> expected { 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1 };
    yuki::utility::CyclicContainerAdaptor<decltype(original)> adaptor
        { original, original.begin() + 2, 2 };
    for(auto &i : adaptor)
    {
        repeated.push_back(i);
    }
    ASSERT_EQ(repeated, expected);
}
