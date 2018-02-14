#include "gtest/gtest.h"

#include <Usagi/Engine/Runtime/Memory/BitmapAllocator.hpp>

TEST(BitmapAllocatorTest, AlignedBaseAddress)
{
    // not aligned
    EXPECT_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(1), 16, 4
        , 4), std::invalid_argument);
    // offset-based
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(0), 16
        , 4, 4));
    // aligned
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(4), 16
        , 4, 4));
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(4), 16
        , 4, 2));
}

TEST(BitmapAllocatorTest, BigEnoughTotalSize)
{
    // zero size
    EXPECT_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(0), 0, 4,
        4), std::invalid_argument);
    // cannot hold even one block
    EXPECT_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(0), 3, 4,
        4), std::invalid_argument);
    // single block
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(1<<10)
        , 4, 4, 4));
    // 4 blocks
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(1<<10)
        , 16, 4, 4));
    // ignored tail region
    {
        yuki::memory::BitmapAllocator tail_not_used(reinterpret_cast<void*>(0),
            18, 4, 4);
        EXPECT_EQ(tail_not_used.managedSize(), 18);
        EXPECT_EQ(tail_not_used.usableSize(), 16);
        EXPECT_NO_THROW(tail_not_used.allocate(16));
        EXPECT_NO_THROW(tail_not_used.deallocate(nullptr));
        EXPECT_NO_THROW(tail_not_used.allocate(16));
        EXPECT_NO_THROW(tail_not_used.deallocate(nullptr));
        EXPECT_THROW(tail_not_used.allocate(18), std::bad_alloc);
    }
}

TEST(BitmapAllocatorTest, MaxAlignment)
{
    // non-power-of-two
    EXPECT_THROW(yuki::memory::BitmapAllocator(nullptr, 16, 4, 3), std::
        invalid_argument);
    // 2^0
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(nullptr, 16, 4, 1));
    // 2^1
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(nullptr, 16, 4, 2));
    // too large
    EXPECT_THROW(yuki::memory::BitmapAllocator(nullptr, 16, 16, 32), std::
        invalid_argument);
    // no alignement
    EXPECT_NO_THROW(yuki::memory::BitmapAllocator(reinterpret_cast<void*>(123), 16, 4, 0));
}

TEST(BitmapAllocatorTest, BasicAlloc)
{
    yuki::memory::BitmapAllocator alloc(nullptr, 16, 4, 4);
    EXPECT_EQ(alloc.allocate(8, 4), reinterpret_cast<void*>(0));
    EXPECT_EQ(alloc.allocate(8, 4), reinterpret_cast<void*>(8));
    EXPECT_NO_THROW(alloc.deallocate(reinterpret_cast<void*>(0)));
    EXPECT_NO_THROW(alloc.deallocate(reinterpret_cast<void*>(8)));
    EXPECT_EQ(alloc.allocate(16, 4), reinterpret_cast<void*>(0));
    EXPECT_NO_THROW(alloc.deallocate(reinterpret_cast<void*>(0)));
    EXPECT_THROW(alloc.allocate(16, 16), std::invalid_argument);
}
