#include "gtest/gtest.h"

#include <Usagi/Engine/Runtime/Memory/CircularAllocator.hpp>

TEST(CircularAllocatorTest, CircularAllocation) {
    char *base = reinterpret_cast<char*>(65536);
    yuki::memory::CircularAllocator alloc(base, 16);

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
