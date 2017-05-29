#include <gtest/gtest.h>

#include <Usagi/Meta/premain.hpp>

static int premain_value = 0;

struct premain_test
{
    premain_test()
    {
        premain_value = 1;
    }
};

template yuki::premain_action<premain_test>;

TEST(premain_test, code_is_executed)
{
    EXPECT_EQ(premain_value, 1);
}
