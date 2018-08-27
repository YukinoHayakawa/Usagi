#include <gtest/gtest.h>

#include <Usagi/Utility/TypeCast.hpp>
#include <Usagi/Utility/RotateCounter.hpp>

using namespace usagi;

namespace
{
struct A { virtual ~A() = default; };
struct B : A { };
struct C { virtual ~C() = default; };
}

TEST(TypeCastTest, IsInstanceOfTest)
{
	B b;
	C c;
	EXPECT_TRUE(is_instance_of<A>(&b));
	EXPECT_TRUE(is_instance_of<B>(&b));
	EXPECT_FALSE(is_instance_of<C>(&b));
	EXPECT_FALSE(is_instance_of<A>(&c));
	EXPECT_FALSE(is_instance_of<B>(&c));
	EXPECT_TRUE(is_instance_of<C>(&c));
}

TEST(TypeCastTest, IsInstanceOfConstTest)
{
	const B b;
	const C c;
	EXPECT_TRUE(is_instance_of<A>(&b));
	EXPECT_TRUE(is_instance_of<B>(&b));
	EXPECT_FALSE(is_instance_of<C>(&b));
	EXPECT_FALSE(is_instance_of<A>(&c));
	EXPECT_FALSE(is_instance_of<B>(&c));
	EXPECT_TRUE(is_instance_of<C>(&c));
}

TEST(RotateCounterTest, zeroRange)
{
    RotateCounter<int> a;

    EXPECT_EQ(a.current(), 0);
    EXPECT_EQ(a.max(), 0);
    EXPECT_EQ(a++, 0);
    EXPECT_EQ(++a, 0);
    a.reset(0);
    EXPECT_EQ(++a, 0);
    EXPECT_EQ(a++, 0);

    RotateCounter<unsigned> b;

    EXPECT_EQ(b.current(), 0);
    EXPECT_EQ(b.max(), 0);
    EXPECT_EQ(b++, 0);
    EXPECT_EQ(++b, 0);
    b.reset(0);
    EXPECT_EQ(++b, 0);
    EXPECT_EQ(b++, 0);
}

TEST(RotateCounterTest, wrap)
{
    RotateCounter<int> a(3);

    EXPECT_EQ(a.current(), 0);
    EXPECT_EQ(a.max(), 3);
    EXPECT_EQ(a++, 0);
    EXPECT_EQ(a.current(), 1);
    EXPECT_EQ(++a, 2);
    EXPECT_EQ(a.current(), 2);
    EXPECT_EQ(a++, 2);
    EXPECT_EQ(a.current(), 0);
}
