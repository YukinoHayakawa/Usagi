#include "MaybeOwnedString.hpp"

namespace usagi
{
MaybeOwnedString::MaybeOwnedString(std::string_view view)
    : mView(view)
{
}

MaybeOwnedString::MaybeOwnedString(const char *str)
    : MaybeOwnedString(std::string_view(str))
{
}

MaybeOwnedString::MaybeOwnedString(const std::string &str)
    : MaybeOwnedString(std::string_view(str))
{
}

MaybeOwnedString::MaybeOwnedString(std::string &&str)
    : mStorage(std::move(str))
    , mView(mStorage.value())
{
}

MaybeOwnedString::MaybeOwnedString(const MaybeOwnedString &other)
    : mStorage { other.mStorage }
    // make sure only refer to own storage
    , mView { mStorage.has_value() ? mStorage.value() : other.mView }
{
}

MaybeOwnedString::MaybeOwnedString(MaybeOwnedString &&other) noexcept
    : mStorage { std::move(other.mStorage) }
    , mView { std::move(other.mView) }
{
    other.mView = std::string_view();
}

MaybeOwnedString & MaybeOwnedString::operator=(const MaybeOwnedString &other)
{
    if(this == &other)
        return *this;
    mStorage = other.mStorage;
    mView = mStorage.has_value() ? mStorage.value() : other.mView;
    return *this;
}

MaybeOwnedString & MaybeOwnedString::operator=(
    MaybeOwnedString &&other) noexcept
{
    if(this == &other)
        return *this;
    mStorage = std::move(other.mStorage);
    mView = std::move(other.mView);
    other.mView = std::string_view();
    return *this;
}
}
