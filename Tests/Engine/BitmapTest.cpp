#include "gtest/gtest.h"

#include <Usagi/Engine/Runtime/Memory/Bitmap.hpp>

class BitmapTest : public ::testing::Test
{
protected:
    BitmapTest()
    {
    }

    yuki::memory::Bitmap bitmap { 16 };
};

TEST_F(BitmapTest, PositiveAllocationAmount)
{
    EXPECT_THROW(bitmap.allocate(0), std::invalid_argument);
}

TEST_F(BitmapTest, TotalFree)
{
    std::size_t alloc = 0;
    EXPECT_NO_THROW(alloc = bitmap.allocate(16));
    EXPECT_EQ(alloc, 0); // alloc success
    EXPECT_THROW(alloc = bitmap.allocate(1), std::bad_alloc);
    bitmap.deallocate(alloc);
    EXPECT_NO_THROW(alloc = bitmap.allocate(16));
}

TEST_F(BitmapTest, ContinuousAllocation)
{
    const std::size_t alloc_unit = 4;
    std::size_t alloc_begin = 0;
    for(std::size_t i = 0; i < alloc_unit; ++i)
    {
        alloc_begin = bitmap.allocate(alloc_unit, alloc_begin);
        EXPECT_EQ(alloc_begin, alloc_unit * i);
    }
    for(std::size_t i = 0; i < alloc_unit; ++i)
    {
        EXPECT_NO_THROW(bitmap.deallocate(alloc_unit * i));
    }
}   

TEST_F(BitmapTest, IgnoreStartHint)
{
    EXPECT_EQ(bitmap.allocate(4, 2), 0);
    EXPECT_EQ(bitmap.allocate(4, 8), 4);
}

TEST_F(BitmapTest, NoWrappedAllocation)
{
    bitmap.allocate(4);
    bitmap.allocate(4);
    bitmap.deallocate(0);
    EXPECT_THROW(bitmap.allocate(12, 8), std::bad_alloc);
}
