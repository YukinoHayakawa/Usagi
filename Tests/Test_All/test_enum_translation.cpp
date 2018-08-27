#include <gtest/gtest.h>

#include <Usagi/Utility/EnumTranslator.hpp>

enum class A
{
    a, b, c, d, e
};

enum class B
{
    e, d, c, b, a
};

USAGI_ENUM_TRANSLATION(
    A, B, 4,
    (A::a, A::b, A::c, A::d),
    (B::a, B::b, B::c, B::d)
)

TEST(EnumTranslationTest, BasicTranslationTest)
{
    using namespace usagi;

    // todo c++20 constexpr std::find
    /*
    static_assert(translate(A::a) == B::a);
    static_assert(translate(A::b) == B::b);
    static_assert(translate(A::c) == B::c);
    static_assert(translate(A::d) == B::d);
    */

    EXPECT_EQ(translate(A::a), B::a);
    EXPECT_EQ(translate(A::b), B::b);
    EXPECT_EQ(translate(A::c), B::c);
    EXPECT_EQ(translate(A::d), B::d);
    EXPECT_THROW(translate(A::e), std::runtime_error);

    EXPECT_EQ(translate(B::a), A::a);
    EXPECT_EQ(translate(B::b), A::b);
    EXPECT_EQ(translate(B::c), A::c);
    EXPECT_EQ(translate(B::d), A::d);
}
