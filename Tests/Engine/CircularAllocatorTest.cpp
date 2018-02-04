#include "gtest/gtest.h"

#include <random>

#include <Usagi/Engine/Runtime/Memory/CircularAllocator.hpp>

class CircularAllocatorTest : public ::testing::Test
{
protected:
    CircularAllocatorTest()
    {
    }

    char *base = reinterpret_cast<char*>(65536);
    yuki::memory::CircularAllocator alloc { base, 16 };
};

TEST_F(CircularAllocatorTest, CircularAllocation)
{
    EXPECT_EQ(alloc.allocate(4, 0), base + 0);
    EXPECT_EQ(alloc.allocate(4, 0), base + 4);
    EXPECT_EQ(alloc.allocate(4, 0), base + 8);
    EXPECT_EQ(alloc.allocate(4, 0), base + 12);
    EXPECT_THROW(alloc.allocate(4, 0), std::bad_alloc);

    EXPECT_NO_THROW(alloc.deallocate(base + 12));
    EXPECT_NO_THROW(alloc.deallocate(base + 8));
    EXPECT_NO_THROW(alloc.deallocate(base + 4));
    EXPECT_NO_THROW(alloc.deallocate(base + 0));

    EXPECT_EQ(alloc.allocate(16, 0), base + 0);
}

TEST_F(CircularAllocatorTest, Alignment)
{
    EXPECT_EQ(alloc.allocate(4, 7), base + 5);
    EXPECT_EQ(alloc.allocate(4, 0), base + 9);
    EXPECT_THROW(alloc.allocate(4, 0), std::bad_alloc);
    EXPECT_EQ(alloc.allocate(3, 0), base + 13);

    EXPECT_NO_THROW(alloc.deallocate(base + 9));
    EXPECT_NO_THROW(alloc.deallocate(base + 5));
    EXPECT_NO_THROW(alloc.deallocate(base + 13));

    EXPECT_EQ(alloc.allocate(16, 0), base + 0);
}

TEST_F(CircularAllocatorTest, WrapToBegin)
{
    EXPECT_EQ(alloc.allocate(8, 0), base + 0);
    EXPECT_EQ(alloc.allocate(4, 0), base + 8);
    EXPECT_NO_THROW(alloc.deallocate(base + 0));
    EXPECT_EQ(alloc.allocate(8, 0), base + 0);
    EXPECT_NO_THROW(alloc.deallocate(base + 0));
    EXPECT_NO_THROW(alloc.deallocate(base + 8));

    EXPECT_EQ(alloc.allocate(16, 0), base + 0);
    EXPECT_THROW(alloc.allocate(1, 0), std::bad_alloc);
    EXPECT_NO_THROW(alloc.deallocate(base + 0));
}

TEST(CircularAllocatorPressureTest, RandomUsage)
{
    char *base = reinterpret_cast<char*>(65536);
    const std::size_t total = 1024 * 1024; // 1MB
    yuki::memory::CircularAllocator alloc { base, total };

    std::deque<void*> ptr;
    std::set<void*> unique;
    std::mt19937 gen(0);
    std::uniform_int_distribution<> allocate(0, 2);
    std::uniform_int_distribution<> size_dis(1, 1024);
    std::uniform_int_distribution<> align_dis(0, 64);

    for(int i = 0; i < 10000; ++i)
    {
        if(!allocate(gen) && !ptr.empty())
        {
            std::uniform_int_distribution<> idx_dis(0, ptr.size() - 1);
            auto idx = idx_dis(gen);
            auto iter = ptr.begin() + idx;
            EXPECT_NO_THROW(alloc.deallocate(*iter));
            unique.erase(*iter);
            ptr.erase(iter);
        }
        else
        {
            try
            {
                auto size = size_dis(gen);
                auto new_ptr = alloc.allocate(size, align_dis(gen));
                ASSERT_EQ(unique.find(new_ptr), unique.end());
                ptr.push_back(new_ptr);
                unique.insert(new_ptr);
            }
            catch(...)
            {
            }
        }
    }

    for(auto p : ptr)
    {
        EXPECT_NO_THROW(alloc.deallocate(p));
    }

    EXPECT_EQ(alloc.allocate(total, 0), base);
}
