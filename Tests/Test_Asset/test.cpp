#include <gtest/gtest.h>

#include <Usagi/Engine/Asset/AssetPackage.hpp>

using namespace yuki;

namespace
{
struct A
{
    virtual ~A() = default;
};

struct B : A
{
};

class TestAssetPackage : public AssetPackage
{
    std::any getAssetAny(const std::string &name) override
    {
        if(name == "A")
        {
            return std::make_shared<A>();
        }
        if(name == "B")
        {
            return std::static_pointer_cast<A>(std::make_shared<B>());
        }
        if(name == "int")
        {
            return std::make_shared<int>(1);
        }
        throw std::runtime_error("No such asset.");
    }
};
}

TEST(AssetTest, AssetTypeCastTest)
{
    TestAssetPackage p;

    // identical
    EXPECT_NO_THROW(p.getAsset<A>("A"));
    // identical
    // macro + template = errornous param separation
    EXPECT_NO_THROW((p.getAsset<A, A>("A")));
    // invalid upcast
    EXPECT_THROW((p.getAsset<A, B>("A")), std::bad_cast);

    // stored type
    EXPECT_NO_THROW(p.getAsset<A>("B"));
    // invalid
    EXPECT_THROW(p.getAsset<B>("B"), std::bad_any_cast);
    // upcast
    EXPECT_NO_THROW((p.getAsset<A, B>("B")));
    // same as stored type
    EXPECT_NO_THROW((p.getAsset<A, A>("B")));

    EXPECT_THROW(p.getAsset<A>("int"), std::bad_any_cast);
    EXPECT_THROW((p.getAsset<A, B>("int")), std::bad_any_cast);
    EXPECT_THROW((p.getAsset<A, A>("int")), std::bad_any_cast);
    EXPECT_NO_THROW(p.getAsset<int>("int"));
    EXPECT_EQ(*p.getAsset<int>("int"), 1);
}

#include "../GTestMain.hpp"
