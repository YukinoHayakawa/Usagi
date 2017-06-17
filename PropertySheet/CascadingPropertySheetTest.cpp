#include <gtest/gtest.h>

#include <Usagi/PropertySheet/CascadingPropertySheet.hpp>
#include <gtest/gtest.h>

using namespace yuki::property_sheet;

class SingleLayerCascadingPropertySheetTest : public ::testing::Test
{
protected:
    CascadingPropertySheet sheet;
};

TEST_F(SingleLayerCascadingPropertySheetTest, duplicate_key_throw)
{
    sheet.insert("foo", "deadbeef");
    EXPECT_THROW(sheet.insert("foo", 0xDEADBEEF), DuplicateKeyException);
}

TEST_F(SingleLayerCascadingPropertySheetTest, work_with_different_types)
{
    sheet.insert("A", std::make_any<std::string>("beef"));
    sheet.insert("B", "another beef");
    sheet.insert("C", 1);
}

TEST_F(SingleLayerCascadingPropertySheetTest, get_back_correct_type)
{
    sheet.insert("A", std::make_any<std::string>("beef"));
    EXPECT_NO_THROW(sheet.find<std::string>("A"));
}

TEST_F(SingleLayerCascadingPropertySheetTest, throw_if_not_found)
{
    EXPECT_THROW(sheet.find<int>("no"), KeyNotFoundException);
}

TEST_F(SingleLayerCascadingPropertySheetTest, can_erase)
{
    sheet.insert("C", 1);
    EXPECT_TRUE(sheet.erase("C"));
}

class MultipleLayerCascadingPropertySheetTest : public ::testing::Test
{
protected:
    CascadingPropertySheet root, middle { &root }, bottom { &middle };
};

TEST_F(MultipleLayerCascadingPropertySheetTest, value_overridden)
{
    root.insert("foo", 1);
    EXPECT_EQ(bottom.find<int>("foo"), 1);

    middle.insert("foo", 2);
    EXPECT_EQ(bottom.find<int>("foo"), 2);

    bottom.insert("foo", 3);
    EXPECT_EQ(bottom.find<int>("foo"), 3);
}

TEST_F(MultipleLayerCascadingPropertySheetTest, value_cascaded)
{
    root.insert("foo", true);
    EXPECT_EQ(bottom.find<bool>("foo"), true);
}

TEST_F(MultipleLayerCascadingPropertySheetTest, cannot_erase_from_parent)
{
    root.insert("foo", "bar");
    EXPECT_FALSE(bottom.erase("foo"));
}
